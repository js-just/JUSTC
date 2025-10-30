require('./justc.node.js');
(async ()=>{return await require('./justc.js')})().then((JUSTC)=>{
    console.log(typeof JUSTC, JUSTC, typeof JUSTC.initialize);
    JUSTC.initialize().then(()=>{
        console.log(JSON.stringify(JUSTC.execute('a is 123456.')))
    });
})
