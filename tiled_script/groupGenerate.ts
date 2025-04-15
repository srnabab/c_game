/// <reference types="@mapeditor/tiled-api" />

class Point1
{
    x: number = 0;
    y: number = 0;

    constructor (x: number, y: number)
    {
        this.x = x;
        this.y = y;
    }

    isEqual(b: Point1): boolean
    {
        if ((this.x === b.x) && (this.y === b.y))
        {
            return true;
        }

        return false;
    }
}
class PointID extends Point1
{
    id: number = -1;

    constructor (x: number, y: number, id: number)
    {
        super(x, y);

        this.id = id;
    }

    getXY(): Point1
    {
        return new Point1(this.x, this.y);
    }
}

class AABB
{
    x: number = 0;
    y: number = 0;
    width: number = 0;
    height: number = 0;

    constructor (x: number, y: number, width: number, height: number);
    constructor (obj: MapObject);
    constructor (first: number | MapObject, y?: number, width?: number, height?: number)
    {
        if (typeof first === "number")
        {
            this.x = first;
            this.y = y!;
            this.width = width!;
            this.height = height!;
        }
        else
        {
            const obj: MapObject = first as MapObject;
            this.x = obj.x;
            this.y = obj.y;
            this.width = obj.width;
            this.height = obj.height;
        }
    }

    rectangleIntersectRegion(x: number, y: number, width: number, height: number): boolean;
    rectangleIntersectRegion(obj: MapObject): boolean;
    rectangleIntersectRegion(first: number | MapObject, y?: number, width?: number, height?: number): boolean 
    {
        if (typeof first === 'number')
        {
            const objRight: number = this.x + this.width;
            const objBottom: number = this.y + this.height;
            const regionRight: number = first + width!;
            const regionBottom: number = y! + height!;

            const noIntersection: boolean = objRight <= first|| this.x >= regionRight || objBottom <= y!|| this.y >= regionBottom;

            return !noIntersection;
        }
        else
        {
            const objRight: number = this.x + this.width;
            const objBottom: number = this.y + this.height;
            const regionRight: number = first.x + first.width;
            const regionBottom: number = first.y + first.height;

            const noIntersection: boolean = objRight <= first.x|| this.x >= regionRight || objBottom <= first.y!|| this.y >= regionBottom;

            return !noIntersection;
        }
    }
}
function checkRegionForObjects(objectLayer: ObjectGroup, group: MapObject): number;
function checkRegionForObjects(objectLayer: ObjectGroup, regionX: number, regionY: number, regionWidth: number, regionHeight: number): number;
function checkRegionForObjects(objectLayer: ObjectGroup, second: number | MapObject, regionY?: number, regionWidth?: number, regionHeight?: number): number{
    if (!objectLayer || !objectLayer.isObjectLayer) 
    {
        tiled.log("Provided layer is not a valid ObjectGroup.");
        return -1;
    }

    const objects = objectLayer.objects;
    if (!objects) 
    {
        return -1;
    }

    let regionX:number = 0;
    let group: MapObject | null = null;
    if (typeof second === 'number') regionX = second;
    else group = second as MapObject;


    for (const obj of objects) 
    {
        let tempAABB: AABB = new AABB(obj);
        if (group != null)
        {
            if (tempAABB.rectangleIntersectRegion(group)) 
            {
                // tiled.log(`Found intersecting object: ID=${obj.id}, Name='${obj.name}'`);
                return obj.id;
            }
        }
        else
        {
            if (tempAABB.rectangleIntersectRegion(regionX!, regionY!, regionWidth!, regionHeight!)) 
            {
                // tiled.log(`Found intersecting object: ID=${obj.id}, Name='${obj.name}'`);
                return obj.id;
            }
        }
    }

    tiled.log("No objects intersect the specified region in this layer.");
    return -1;
}
function findMapObj(objectLayer: ObjectGroup, id: number): MapObject | null
{
    if (!objectLayer || !objectLayer.isObjectLayer) 
    {
        tiled.log("Provided layer is not a valid ObjectGroup.");
        return null;
    }

    const objects = objectLayer.objects;
    if (!objects) 
    {
        return null;
    }

    for (const obj of objects) 
    {
        if (obj.id == id) return obj;
    }

    return null;
}
var action = tiled.registerAction("CustomAction", function(action) {

    let asset: Asset | null = tiled.activeAsset;
    if (asset == null) 
    {
        return;
    }

    if (!asset.isTileMap)
    {
        return;
    }

    const currentMap: TileMap = asset as TileMap;

    const regionSizeProperty: TiledObjectPropertyValue = currentMap.property("regionSize");
    let regionSize: number = 0;
    if (typeof regionSizeProperty === 'number') regionSize = regionSizeProperty as number;
    else if (typeof regionSizeProperty === 'undefined')
    {
        currentMap.macro("add regionSize", function() {
            currentMap.setProperty("regionSize", 50);
        });
        regionSize = 50;
    }

    const tileSize: number = currentMap.tileWidth;

    if (currentMap.layers.length > 2 || currentMap.layers.length == 0)
    {
        return;
    }

    if (currentMap.layers.length == 1)
    {
        if (currentMap.layers[0].isTileLayer)
        {
            let tempObjectLayer: ObjectGroup = new ObjectGroup();
            tempObjectLayer.name = 'group object layer';
            currentMap.addLayer(tempObjectLayer);
        }
        else return;
    }

    let tileLayer: TileLayer = new TileLayer();
    let objectLayer: ObjectGroup = new ObjectGroup();

    let layer0: Layer = currentMap.layerAt(0);
    let layer1: Layer = currentMap.layerAt(1);

    if (layer0.isTileLayer) tileLayer = layer0 as TileLayer;
    else if (layer0.isObjectLayer) objectLayer = layer0 as ObjectGroup;
    else return;

    if (layer1.isTileLayer) tileLayer = layer1 as TileLayer;
    else if (layer1.isObjectLayer) objectLayer = layer1 as ObjectGroup;
    else return;

    let mapRegion: region = tileLayer.region();
    let rectangleCount: number = mapRegion.rects.length;

    tiled.log(`Processing map: ${currentMap.fileName}`);
    tiled.log(`Map size: ${currentMap.width}x${currentMap.height}`);
    tiled.log(`Number of layers: ${currentMap.layerCount}`);
    tiled.log(`region size: ${regionSize}`);
    tiled.log(`region count:${rectangleCount}`);

    let pointArrays: Array<PointID> = [];

    mapRegion.rects.forEach(rectangle => {
        let startX: number = rectangle.x;
        let startY: number = rectangle.y;

        let bottomRowCount: number = Math.floor(rectangle.height / regionSize);
        let bottomColCount: number = Math.floor(rectangle.width / regionSize);

        // let endX: number = bottomColCount * regionSize + startX;
        // let endY: number = bottomRowCount * regionSize + startY;

        for (let i = 0;i < bottomRowCount;i++)
        {
            for (let j = 0;j < bottomColCount;j++)
            {
                let group: MapObject = new MapObject(MapObject.Rectangle, "group");
                group.visible = true;
                group.x = (startX + j * regionSize) * tileSize;
                group.y = (startY + i * regionSize) * tileSize;
                group.width = tileSize * regionSize;
                group.height = group.width;
                group.rotation = 0.0;

                let id: number = checkRegionForObjects(objectLayer, group);
                if (id != -1)
                {
                    let tempMapObj: MapObject | null = findMapObj(objectLayer, id);

                    // tiled.log(`id: ${id}`);

                    if (tempMapObj != null)
                    {
                        pointArrays.push(new PointID(tempMapObj.x, tempMapObj.y, id));
                    }
 
                    continue;
                }
                else
                {
                    objectLayer.addObject(group);

                    pointArrays.push(new PointID(group.x, group.y, objectLayer.objects[objectLayer.objectCount - 1].id));
                }
            }
        }

        // tiled.log(`rectangel width: ${rectangle.width}`);
        // tiled.log(`rectangel height: ${rectangle.height}`);
        // tiled.log(`start (${startX}, ${startY})`);
        // tiled.log(`bottom row: ${bottomRowCount}`);
        // tiled.log(`bottom col: ${bottomColCount}`);
    });

    // pointArrays.forEach(point => {
    //     tiled.log(`point(${point.x}, ${point.y}, id: ${point.id})`);
    // });

    objectLayer.objects.forEach(tempMapObj => {
 
        let upPoint: Point1 = new Point1(tempMapObj.x, tempMapObj.y - 800);
        let downPoint: Point1 = new Point1(tempMapObj.x, tempMapObj.y + 800);
        let leftPoint: Point1 = new Point1(tempMapObj.x - 800, tempMapObj.y);
        let rightPoint: Point1 = new Point1(tempMapObj.x + 800, tempMapObj.y);

        let upID: PointID | undefined = pointArrays.find(pointArray => pointArray.isEqual(upPoint));
        let downID: PointID | undefined = pointArrays.find(pointArray => pointArray.isEqual(downPoint));
        let leftID: PointID | undefined = pointArrays.find(pointArray => pointArray.isEqual(leftPoint));
        let rightID: PointID | undefined= pointArrays.find(pointArray => pointArray.isEqual(rightPoint));

        tempMapObj.setProperty("up", upID != undefined ? upID.id : -1);
        tempMapObj.setProperty("down", downID != undefined ? downID.id : -1);
        tempMapObj.setProperty("left", leftID != undefined ? leftID.id : -1);
        tempMapObj.setProperty("right", rightID != undefined ? rightID.id : -1);

        // tiled.log(`up point ${upPoint.x}, ${upPoint.y}`)
        // tiled.log(`up point ${rightPoint.x}, ${rightPoint.y}`)
        // tiled.log(`up point ${leftPoint.x}, ${leftPoint.y}`)
        // tiled.log(`right point ${rightPoint.x}, ${rightPoint.y}`)
        // tiled.log(`obj id${tempMapObj.id}`);
    });
    tiled.log(action.text + " was " + (action.checked ? "checked" : "unchecked"))
})

action.text = "generate group"
action.checkable = true
action.enabled = true