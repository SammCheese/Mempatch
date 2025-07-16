/// <reference types="node" />



/**
  * Patches memory in the current process.
  * 
  * @param pattern The pattern to search for in memory.
  * @param patternMask Mask for the pattern where 1 matches bytes and 0 ignores them.
  * @param replacement The replacement bytes to write.
  * @param replacementMask A mask where 1means overwrite, 0 means keep original.
  * 
  * @returns The address where the patch was applied, or `null` if not found.
  */
export function patch(
  pattern: Buffer,
  patternMask: Buffer,
  replacement: Buffer,
  replacementMask: Buffer
): number | null;
  
/**
  * @param {string} sig - The signature to convert to buffers. e.g. "C6 43 ?? ??"
  * @returns {{ bytes: Buffer, mask: Buffer }} - An object containing the byte buffer and mask buffer.
  */
export function sigToBufs(sig: string): {
  bytes: Buffer;
  mask: Buffer;
};

declare const _default: {
  patch: typeof patch;
  sigToBufs: typeof sigToBufs;
};

export default _default;

