let wasmModule;

async function initWasm() {
    try {
        wasmModule = await __JUSTC__();
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

function displayLogs(result) {
    if (result.logs && Array.isArray(result.logs)) {
        result.logs.forEach(log => {
            if (log.type === 'ECHO' || log.type === 'LOG') {
                console.log(`[${log.type}] ${log.message}`);
            } else if (log.type === 'ERROR') {
                console.error(`[${log.type}] ${log.message}`);
            }
        });
    }
    
    if (result.logfile && result.logfile.file) {
        console.log(`Log file: ${result.logfile.file}`);
        if (result.logfile.logs) {
            console.log('Log file content:', result.logfile.logs);
        }
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
            const resultjson = wasmModule.UTF8ToString(resultptr);
            wasmModule.ccall(
                'free_string',
                null,
                ['number'],
                [resultptr]
            );
            
            const result = JSON.parse(resultjson);
            
            if (!noAlert) {
                alert(JSON.stringify(result, null, 2));
            }
            
            console.log("Lexer result:", result);
            return resultjson;
            
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
    
    if (!input) {
        alert("Cancelled: No input provided.");
        return;
    }
    
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
        
        const result = JSON.parse(resultJson);
        
        displayLogs(result);
        
        alert(JSON.stringify(result, null, 2));
        
        return result;
        
    } catch (error) {
        console.error("Error:", error);
        alert("JUSTC/core/parser.cpp error.");
    }
}

async function executeJUSTC(code) {
    await checkWASM();
    
    try {
        const resultPtr = wasmModule.ccall(
            'parse',
            'number',
            ['string'],
            [code]
        );
        
        const resultJson = wasmModule.UTF8ToString(resultPtr);
        wasmModule.ccall('free_string', null, ['number'], [resultPtr]);
        
        const result = JSON.parse(resultJson);
        displayLogs(result);
        
        return result;
        
    } catch (error) {
        console.error("Execution error:", error);
        throw error;
    }
}

initWasm();
