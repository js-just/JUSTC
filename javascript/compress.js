const [file] = process.argv.slice(2);
const fs = require('fs');
const JSSC = require('strc');
const enabled = false;

if (enabled) {
    try {
        const utf8Buffer = fs.readFileSync(file);
        const utf16String = utf8Buffer.toString('utf8');

        console.log(utf16String);
        const compressed = JSSC.compress(utf16String);

        console.log(compressed);
        fs.writeFileSync(file, compressed, 'utf16le');

    } catch (error) {
        console.log('::error::', file, error);
    }
}
