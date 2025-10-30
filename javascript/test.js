require('./justc.node.js');
const JUSTC = require('./justc.js');
console.log(typeof JUSTC, JUSTC, typeof JUSTC.initialize);
JUSTC.initialize().then(()=>{
    console.log(JSON.stringify(JUSTC.execute('a is 123456.')))
});
