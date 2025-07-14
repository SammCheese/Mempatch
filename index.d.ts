/// <reference types="node" />

declare module "mempatch" {

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
}