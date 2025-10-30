const JUSTCWASM = require('./justc.node.js');
(async ()=>{return await require('./justc.js')})().then((JUSTC)=>{
    JUSTC.defineWASM(JUSTCWASM);
    console.log(typeof JUSTC, JUSTC, typeof JUSTC.initialize);
    JUSTC.initialize().then(()=>{
        console.log(JSON.stringify(JUSTC.execute('a is 123456.')))
    });
})
