# Mempatch (win32/linux)

**Runtime memory patching for Node.js/Electron apps using byte signatures** 

Did you ever want to Patch out some unwanted things out of (your own) Electron/Nodejs Application? Well now you can! 


This little tool allows you to pattern-replace a piece of memory in the process that this is run in! (OSX not included)

> ⚠️ **Only supports Windows (untested) and Linux for now.**
>
> macOS is not supported. 

## Installation

```bash
(npm/pnpm) i
(npm/pnpm) run build
```


## Use Cases

- Disable Hardcoded flags in Electron apps
- Modify Behavior in Modules
- Patch telemetry in your own tools


## Example Usage

```js
const { patch } = require("mempatch")

// The pattern to be searched for in memory
// Wildcards (??) will match anything
const pattern =
  "C6 43 ?? ?? C6 43 ?? ?? 48 83 C4 ?? 5B 41 5C";
// The pattern that will replace the above pattern
// Wildcards (??) will not be replaced
const replace =
  "90 90 90 90 90 90 90 90 48 83 C4 ?? 5B 41 5C";

/**
 * @param {string} sig - The signature to convert to buffers.
 * @returns {{ bytes: Buffer, mask: Buffer }} - An object containing the byte buffer and mask buffer.
 */
function sigToBufs(sig) {
  const parts = sig.trim().split(/\s+/);
  const bytes = Buffer.alloc(parts.length);
  const mask  = Buffer.alloc(parts.length);

  parts.forEach((part, i) => {
    if (part === "??" || part === "?") {
      bytes[i] = 0x00;
      mask[i]  = 0;
    } else {
      bytes[i] = parseInt(part, 16);
      mask[i]  = 1;
    }
  });

  return { bytes, mask };
}

const { bytes: patBuf , mask: patMask } = sigToBufs(pattern);
const { bytes: replBuf, mask: replMask } = sigToBufs(replace);

// Perform the patch
const addr = patch(patBuf, patMask, replBuf, replMask);

// If no address is returned, the patch failed
if (!addr) {
  throw new Error("Patch Failed - pattern not found.");
} else {
  console.log(`Patch applied at 0x${addr.toString(16)}`)
}
```

## Disclaimer

This tool is intended for *education and debugging purposes only*
Use it responsibly and *only on software you own or are authorized to modify.*