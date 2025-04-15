/// <reference types="@mapeditor/tiled-api" />

tiled.registerTilesetFormat("G tileset format", {
    name: "G tileset export(*.tsd)",

    extension: "tsd",

    write: function(tileset: Tileset, fileName: string): string | undefined{
        
        tiled.log(`begin export ${fileName}`);
 
        let properties: TiledObjectProperties = tileset.tiles[0].properties();
        let tilePropertiesCount: number = Object.keys(properties).length;
       
        let rowCount: number = tileset.tileCount / tileset.columnCount;
        let indexByteCount: number = tileset.tileCount * (4 + tilePropertiesCount);

        let buffer: ArrayBuffer = new ArrayBuffer(28 + indexByteCount)
        let dataView: DataView= new DataView(buffer);

        let logMessage: string = '';
        logMessage += `row count: ${rowCount}\n`;
        logMessage += `col count: ${tileset.columnCount}\n`;
        logMessage += `tile width: ${tileset.tileWidth}\n`;
        logMessage += `tile height: ${tileset.tileHeight}\n`;
        logMessage += `index byte count: ${indexByteCount}\n`;
        logMessage += `properties count: ${tilePropertiesCount}\n`;
        tiled.log(logMessage);

        let u8Offset: number = 0;
        dataView.setUint8(u8Offset, 't'.charCodeAt(0)); u8Offset++;
        dataView.setUint8(u8Offset, 's'.charCodeAt(0)); u8Offset++;
        dataView.setUint8(u8Offset, 'd'.charCodeAt(0)); u8Offset++;
        // dataView.setUint8(u8Offset, 'I'.charCodeAt(0)); u8Offset++;
        dataView.setUint8(u8Offset, 99); u8Offset++;

        dataView.setUint32(u8Offset, tileset.imageWidth, true); u8Offset += 4;
        dataView.setUint32(u8Offset, tileset.imageHeight, true); u8Offset += 4;
        dataView.setUint32(u8Offset, tileset.tileWidth, true); u8Offset += 4;
        dataView.setUint32(u8Offset, tileset.tileHeight, true); u8Offset += 4;
        dataView.setUint32(u8Offset, tilePropertiesCount, true); u8Offset += 4;
        dataView.setUint32(u8Offset, indexByteCount, true); u8Offset += 4;

        let j: number = 0;
        let i: number = 0;

        for (j = 0;j < tileset.tileCount;j++)
        {
            dataView.setUint32(u8Offset, tileset.tiles[j].id, true); u8Offset += 4;
            
            if (tilePropertiesCount != 0)
            {
                let tempProperties: TiledObjectProperties = tileset.tiles[j].properties();
                for (const [key, tempPropertiesVal] of Object.entries(tempProperties)) 
                {
                    const type = typeof tempPropertiesVal;
            
                    tiled.log(`Processing property "${key}":`);
            
                    if (type === 'boolean') {
                        let tempVal: number = tempPropertiesVal ? 1 : 0;
                        dataView.setUint8(u8Offset, tempVal);
                        tiled.log(`  Value (boolean as uint8): ${tempVal}`);
                        u8Offset += 1;
                    } else if (type === 'number') {
                        tiled.log(`  Value (number): ${tempPropertiesVal}`);
                        // 处理 number...
                    } else if (type === 'string') {
                        tiled.log(`  Value (string): ${tempPropertiesVal}`);
                        // 处理 string...
                    } else if (type === 'object') {
                        tiled.log(`  Value (object): ${JSON.stringify(tempPropertiesVal)}`);
                        // 处理 object...
                    } else if (type === 'undefined') {
                        tiled.log(`  Value is undefined.`);
                    }
                }
            }
        }

        let tsdiFile: BinaryFile = new BinaryFile(fileName, BinaryFile.WriteOnly);
        tsdiFile.write(buffer);

        tsdiFile.commit();

        tiled.log(`done ${Date.now()}`);

        return undefined; // Return a value to satisfy the function's declared type
    },
})

