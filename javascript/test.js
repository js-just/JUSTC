const JUSTCWASM = require('./justc.node.js');
(async()=>{return await require('./justc.js')})().then((JUSTC)=>{
    JUSTC.defineWASM(JUSTCWASM);
    console.log(typeof JUSTC, JUSTC, typeof JUSTC.initialize);
    JUSTC.initialize().then(()=>{
        JUSTC.execute('a is 123, b is a+123, ECHO(b).').then((result)=>{
            console.log(JSON.stringify(result[0]))
        })
    })
})

/* 

const __justc__=require('./justc.node.js');
(async()=>{return await require('./justc.js')})().then(async(JUSTC)=>{
    // YOUR CODE HERE
});

*/
