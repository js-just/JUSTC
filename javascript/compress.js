const [file] = process.argv.slice(2);
const fs = require('fs');
const JSSC = require('strc');

try {
    const content = fs.readFileSync(file, 'utf8');
    const compressed = JSSC.compress(content);
    fs.writeFileSync(file, compressed);
} catch (error) {
    console.error(`Error processing ${file}:`, error.message);
    process.exit(1);
}
