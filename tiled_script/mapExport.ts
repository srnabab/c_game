/// <reference types="@mapeditor/tiled-api" />

class Point
{
    x: number = 0;
    y: number = 0;

    constructor (x: number, y: number)
    {
        this.x = x;
        this.y = y;
    }

    isEqual(b: Point): boolean
    {
        if ((this.x === b.x) && (this.y === b.y))
        {
            return true;
        }

        return false;
    }
}

class Map5IDs
{
    ID: number = -1;
    upID: number = -1;
    downID: number = -1;
    leftID: number = -1;
    rightID: number = -1;

    constructor ();
    constructor (ID: number, upID: number, downID: number, leftID: number, rightID: number)
    constructor (ID?: number, upID?: number, downID?: number, leftID?: number, rightID?: number)
    {
        if (typeof ID === 'number')
        {
            this.ID = ID!;
            this.upID = upID!;
            this.downID = downID!;
            this.leftID = leftID!;
            this.rightID = rightID!;
        }
    }
}

tiled.registerMapFormat("G tilemap format", {
    name: "G tilemap format(*.tsdI)",

    extension: "tsdI",

    write: function(map, fileName) {

        tiled.log("Starting export to: " + fileName);

        if (map.layerCount != 2) return "Error: This exporter only support 2 layers(1 object layer, 1 tile layer)"

        if (map.infinite == false) 
        {
            return "Error: not support finite map";
        }

        let regionSize: number = map.property("regionSize") as number;
        let tileSize: number = map.tileWidth;

        let tileLayer: TileLayer = new TileLayer();
        let objectLayer: ObjectGroup = new ObjectGroup();

        let layer0: Layer = map.layerAt(0);
        let layer1: Layer = map.layerAt(1);

        if (layer0.isTileLayer) tileLayer = layer0 as TileLayer;
        else if (layer0.isObjectLayer) objectLayer = layer0 as ObjectGroup;

        if (layer1.isTileLayer) tileLayer = layer1 as TileLayer;
        else if (layer1.isObjectLayer) objectLayer = layer1 as ObjectGroup;

        let buffer1: ArrayBuffer = new ArrayBuffer(20);
        let buffer1u8View: Uint8Array = new Uint8Array(buffer1);
        let buffer1u32View: Uint32Array = new Uint32Array(buffer1);

        buffer1u8View[0] = 't'.charCodeAt(0);
        buffer1u8View[1] = 's'.charCodeAt(0);
        buffer1u8View[2] = 'd'.charCodeAt(0);
        buffer1u8View[3] = 'I'.charCodeAt(0);

        buffer1u32View[1] = regionSize;
        buffer1u32View[2] = regionSize;

        let file: BinaryFile = new BinaryFile(fileName, BinaryFile.WriteOnly);

        file.write(buffer1);

        let mapIDs: Array<Map5IDs> = new Array();

        let groupCount: number = 0;
        let groupsDataLen: number = 0;
        let singleGroupsize: number = 20 + regionSize * regionSize * 4;

        let mapRegion: region = tileLayer.region();
        let times: number = 0;

        mapRegion.rects.forEach(rect => {

            let bottomRowCount: number = Math.floor(rect.height / regionSize);
            let bottomColCount: number = Math.floor(rect.width / regionSize);
            // tiled.log(`bottom row count: ${bottomRowCount}`);
            // tiled.log(`bottom col count: ${bottomColCount}`);

            groupCount += bottomColCount * bottomRowCount;
            // tiled.log(`group count: ${groupCount}`);
            let tempBufferSize: number = (bottomColCount * bottomRowCount) * singleGroupsize;
            groupsDataLen += tempBufferSize;

            let tempBuffer: ArrayBuffer = new ArrayBuffer(tempBufferSize);
            let tempBufferU32View: Uint32Array = new Uint32Array(tempBuffer);
            let tempU32Offset: number = 0;

            for (let i = 0;i < bottomRowCount;i++)
            {
                for (let j = 0;j < bottomColCount;j++)
                {
                    tiled.log(`${times} times`);
                    times++;

                    let startX: number = rect.x + j * regionSize;
                    let startY: number = rect.y + i * regionSize;
                    let tempPoint: Point = new Point(startX * tileSize, startY * tileSize);

                    let obj: MapObject | undefined = objectLayer.objects.find(object => new Point(object.x, object.y).isEqual(tempPoint));

                    if (obj === undefined)
                    {
                        tempBufferU32View[tempU32Offset] = -1; tempU32Offset++;
                        tempBufferU32View[tempU32Offset] = -1; tempU32Offset++;
                        tempBufferU32View[tempU32Offset] = -1; tempU32Offset++;
                        tempBufferU32View[tempU32Offset] = -1; tempU32Offset++;
                        tempBufferU32View[tempU32Offset] = -1; tempU32Offset++;
                        tempU32Offset += regionSize * regionSize;

                        continue;
                    }

                    let up: TiledObjectPropertyValue = obj.property("up");
                    let down: TiledObjectPropertyValue = obj.property("down");
                    let left: TiledObjectPropertyValue = obj.property("left");
                    let right: TiledObjectPropertyValue = obj.property("right");

                    tempBufferU32View[tempU32Offset] = obj.id; tempU32Offset++;
                    tempBufferU32View[tempU32Offset] = typeof up != 'number' ? -1 : up; tempU32Offset++;
                    tempBufferU32View[tempU32Offset] = typeof down != 'number' ? -1 : down; tempU32Offset++;
                    tempBufferU32View[tempU32Offset] = typeof left != 'number' ? -1 : left; tempU32Offset++;
                    tempBufferU32View[tempU32Offset] = typeof right != 'number' ? -1 : right; tempU32Offset++;

                    for (let k = startY;k < startY + regionSize;k++)
                    {
                        for (let l = startX;l < startX + regionSize;l++)
                        {
                            let aCell: cell = tileLayer.cellAt(l, k);
                            if (aCell.tileId != -1)
                            {
                                tempBufferU32View[tempU32Offset] = aCell.tileId; tempU32Offset++;
                            }
                            else
                            {
                                tempBufferU32View[tempU32Offset] = 0; tempU32Offset++;
                            }
                        }
                    }
                }
            }

            file.write(tempBuffer);
        });

        buffer1u32View[3] = groupCount;
        buffer1u32View[4] = groupsDataLen;

        file.seek(0);
        file.write(buffer1);

        file.commit();

        tiled.log(`done ${Date.now()}`)

        return undefined;
    }
});