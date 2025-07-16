const native = require("node-gyp-build")(__dirname);

/**
 * @param {string} sig - The signature to convert to buffers. e.g. "C6 43 ?? ??"
 * @returns {{ bytes: Buffer, mask: Buffer }} - An object containing the byte buffer and mask buffer.
 */
function sigToBufs(sig) {
  const parts = sig.trim().split(/\s+/);
  const bytes = Buffer.alloc(parts.length);
  const mask = Buffer.alloc(parts.length);

  parts.forEach((part, i) => {
    if (part === "??" || part === "?") {
      bytes[i] = 0x00;
      mask[i] = 0;
    } else {
      bytes[i] = parseInt(part, 16);
      mask[i] = 1;
    }
  });

  return { bytes, mask };
}

module.exports = {
  patch: native.patch,
  sigToBufs
}
