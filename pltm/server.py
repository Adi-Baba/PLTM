from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
from typing import List
import sys
import os

# Add bindings to path to import pltm_core
sys.path.append(os.path.join(os.path.dirname(__file__), "../bindings"))
from . import PLTM_Engine

app = FastAPI(title="PLTM Memory Server")

# Global Store for active contexts (ID -> Engine)
engines = {}

class InitRequest(BaseModel):
    id: str
    context_size: int = 2048
    s: float = 0.1

class ProcessRequest(BaseModel):
    id: str
    tokens: List[float]

@app.post("/init")
def init_context(req: InitRequest):
    try:
        engines[req.id] = PLTM_Engine(req.context_size, req.s)
        return {"status": "ok", "message": f"Context {req.id} initialized"}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/process")
def process_stream(req: ProcessRequest):
    if req.id not in engines:
        raise HTTPException(status_code=404, detail="Context ID not found")
    
    engine = engines[req.id]
    # For demo, we just return the processed output summary
    # In reality, efficient state management needed
    try:
        output = engine.process(req.tokens)
        
        # We return the "compressed signal" (e.g., the last few values)
        # capable of reconstructing the history if inverted, or just used as-is.
        summary = output[-10:].tolist() # Return last 10 'memory units'
        return {"status": "ok", "memory_summary": summary}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

def start():
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)

if __name__ == "__main__":
    start()
