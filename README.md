# Mempatch (win32/linux)

**Runtime memory patching for Node.js/Electron apps using byte signatures** 

Did you ever want to Patch out some unwanted things out of (your own) Electron/Nodejs Application? Well now you can! 


This little tool allows you to pattern-replace a piece of memory in the process that this is run in! (OSX not included)

> ⚠️ **Only supports Windows and Linux.**
>
> ❌ macOS is not supported. 

## Installation

```bash
(npm/pnpm) i
(npm/pnpm) run build
```


## Use Cases

- Disable Hardcoded flags in Electron apps (e.g. contextIsolation 👀)
- Patch behavior in bundled modules
- Remove telemetry calls from Electron apps


## Usages

`mempatch` exports two (2) functions and works in CommonJS and ESM environments:


- `sigToBufs(sig: string): { bytes: Buffer, mask: Buffer}`
- `patch(pattern, patternMask, replacement, replacementMask): number | null`

### `sigToBufs`

Converts a human-readable signature string into `Buffer` + `mask`. Use `??` or `?` to denote wildcards.

```js
const { sigToBufs } = require("mempatch");

sigToBufs("C6 43 ?? ??");
// => { bytes: <Buffer ...>, mask: <Buffer ...> }
```

### `patch`

Patches the first occurrence of `pattern` in memory with the given `replacement`. Wildcards in the `replacementMask` will preserve the original byte.

```js
const { patch } = require('mempatch');

patch(pattern, patternMask, replacement, replacementMask);
```

## Example

```js
const { patch, sigToBufs } = require("mempatch")

// The pattern to be searched for in memory
// Wildcards (??) will match anything
const pattern =
  "C6 43 ?? ?? C6 43 ?? ?? 48 83 C4 ?? 5B 41 5C";
// The pattern that will replace the above pattern
// Wildcards (??) will not be replaced
const replace =
  "90 90 90 90 90 90 90 90 48 83 C4 ?? 5B 41 5C";

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

>This tool is intended for **education and debugging purposes only**
>
>Use it responsibly and **only on software you own or are authorized to modify.**