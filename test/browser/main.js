let wasmModule;

async function initWasm() {
    try {
        wasmModule = await Module();
        console.log("WASM module loaded.");
    } catch (error) {
        console.error("Failed to load WASM module:", error);
    }
}

async function runLexer() {
    if (!wasmModule) {
        await initWasm();
    }

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
            
            alert(result);
        } catch (error) {
            console.error("Error:", error);
            alert("JUSTC/core/lexer.cpp error.");
        }
    } else {
        alert("Cancelled: No input provided.");
    }
}

initWasm();
