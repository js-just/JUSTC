const [file] = process.argv.slice(2);
const fs = require('fs');
const JSSC = require('strc');

fs.readFile(file).then((content) => {
    fs.writeFileSync(JSSC.compress(content));
});
