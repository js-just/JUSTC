let wasmModule;

async function initWasm() {
    try {
        wasmModule = await Module();
        console.log("WASM module loaded.");
    } catch (error) {
        console.error("Failed to load WASM module:", error);
    }
}

async function checkWASM() {
    if (!wasmModule) {
        await initWasm();
    }
}

async function runLexer(noAlert = false) {
    await checkWASM();

    const inputString = prompt("Enter JUSTC code:");
    
    if (inputString && inputString.length > 0) {
        try {
            const resultptr = wasmModule.ccall(
                'lexer',
                'number',
                ['string'],
                [inputString]
            );
            const resultjson = JSON.parse(wasmModule.UTF8ToString(resultptr));
            wasmModule.ccall(
                'free_string',
                null,
                ['number'],
                resultptr
            );
            const result = JSON.stringify(resultjson);
            
            if (!noAlert) alert(result);
            return result
        } catch (error) {
            console.error("Error:", error);
            alert("JUSTC/core/lexer.cpp error.");
        }
    } else {
        alert("Cancelled: No input provided.");
    }
}

async function runParser() {
    await checkWASM();

    const input = prompt("Enter JUSTC code:");
    
    try {
        const resultPtr = wasmModule.ccall(
            'parse',
            'number',
            ['string'],
            [input]
        );
        
        const resultJson = wasmModule.UTF8ToString(resultPtr);
        
        wasmModule.ccall(
            'free_string',
            null,
            ['number'],
            [resultPtr]
        );
        
        alert(resultJson);
        
    } catch (error) {
        console.error("Error:", error);
        alert("JUSTC/core/parser.cpp error.");
    }
}

initWasm();
