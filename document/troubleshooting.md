# Troubleshooting

## General

### Phi nodes
- The phi node should be *the first instruction of a block*
- Input blocks must be direct predecessor of the phi's block, but for input values it's more permissive.

## Errors
- llvm::LiveVariables::HandleVirtRegUse(unsigned int, llvm::MachineBasicBlock*, llvm::MachineInstr&) () from /usr/lib/x86_64-linux-gnu/libLLVM-8.so.1
=> In a function, a variable from another function is used.
-  Program received signal SIGSEGV, Segmentation fault. 0x00007ffffbf573a2 in llvm::Function::getPersonalityFn() const () from /usr/lib/x86_64-linux-gnu/libLLVM-8.so.1
=> A function tries to throw something but didn't declare `throws` attribute so the main function doesn't setup the personality function.