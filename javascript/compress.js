const [file] = process.argv.slice(2);
console.log('running compressor for', file);
const fs = require('fs');
const JSSC = require('strc');

console.log('reading', file);
try {
    const content = fs.readFileSync(file);
    console.log('compressing', file);
    const compressed = JSSC.compress(content);
    console.log('writing', file);
    fs.writeFileSync(file, compressed, 'utf16le');
    console.log(file, 'compressed successfully');
} catch (error) {
    console.log('::error::', file, error);
}
