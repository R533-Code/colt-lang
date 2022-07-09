# Chunk:
The `Chunk` is the struct responsible of simplifying byte-code writing, globals and string literal serialization.

Now, before explaining how it achieve its role, we need to first understand some of the difficulties it solves:
- Alignment: As every type needs to be aligned (especially when using a pointer to that type), a `Chunk` provides the `ChunkWrite*` and `ChunkGet` API to remove the need to handle low-level alignment handling
- Literals Handling: `lstring` serialization is simplified through sections
- Version Checks: A `Chunk` provides a field where a version (COLT_ABI) is stored, which prohibits a VM to run the byte-code it contains if the VM's and `Chunk`'s ABI is different

### SECTIONS:
A `Chunk` only contains a size, a capacity and a pointer to a byte buffer.
That byte buffer begins with a `HEADER` section containing informations about the `Chunk`.

##### HEADER SECTION:
- uint64_t: ABI, populated in `ChunkInit`
- uint64_t: unused field (reserved for later use)
- uint64_t: GLOBAL SECTION byte offset, or 0 (which means a GLOBAL SECTION does not exist)
- uint64_t: STRING SECTION byte offset, or 0 (which means a STRING SECTION does not exist)
- uint64_t: DEBUG SECTION byte offset, or 0 (which means a DEBUG SECTION does not exist)
- uint64_t: CODE SECTION byte offset, or 0 (which means a CODE SECTION does not exist)

#### GLOBAL SECTION:
This is where each global variable is 'serialized' and stored.
It contains a `QWORD` for each variable, which is the variable's value.

#### STRING SECTION:
This is where each string literal are stored.
- uint64_t: string literal count
- `string literal count` uint64_ts: byte offsets to add to the beginning of the `Chunk`'s byte pointer to obtain the `const char*` to the string.
- a finite number of characters, which are the actual strings literal

#### DEBUG SECTION:
This is where informations about global variables are stored.