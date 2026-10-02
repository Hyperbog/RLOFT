# CODE.1 residual classification ledger

This ledger tracks object-1 bytes that are explicitly classified as **non-translated code bytes** by the current `Code1.cpp`, `PROGRESS.md`, or `STAGING_LEDGER.md`. It is deliberately conservative: a byte is listed only when the current repository evidence explicitly identifies it as alignment/padding/zero-fill or embedded table data.

It does **not** change `kVerifiedTranslatedBytes`. The conservative denominator remains the full object-1 extent (`0x11302` = 70,402 bytes) until all residual bytes are classified. At the checkpoint that created this ledger:

- verified translated code: **69,652 bytes**
- conservative residual: **750 bytes**
- explicitly classified residual below: **677 bytes**
  - embedded tables: **122 bytes**
  - alignment/zero/padding: **443 bytes**
  - proven generic non-code (alignment/data subtype not uniquely recoverable from preserved evidence): **112 bytes**
- residual still requiring classification: **73 bytes**

The `static_assert` interval audit is diagnostic only. Many verified regions are represented without one assert per machine range, so the 5,673-byte assert-gap total is **not** the untranslated-code total.

## Machine-readable classified ranges

Format: `START..END | category | inclusive byte count`. Categories are `alignment`, `table`, and conservative `noncode` where preserved batch arithmetic proves exclusion but does not uniquely recover the padding-vs-data subtype.

<!-- RESIDUAL-RANGES-BEGIN -->
00003..0000F | alignment | 13
00049..0004B | alignment | 3
0043B..0043B | alignment | 1
00649..0064B | alignment | 3
010A2..010A3 | alignment | 2
015AE..015AF | alignment | 2
01A01..01A03 | alignment | 3
01BD1..01BD3 | alignment | 3
01E62..01E63 | alignment | 2
01EE1..01EE3 | alignment | 3
01EFF..01EFF | alignment | 1
01F00..01F23 | table | 36
02197..02197 | alignment | 1
024D1..024D3 | alignment | 3
028E2..028E3 | alignment | 2
02991..02993 | alignment | 3
02A61..02A63 | alignment | 3
02DF9..02DFB | alignment | 3
03135..03137 | alignment | 3
03205..03207 | alignment | 3
0334A..0334B | alignment | 2
037BE..037BF | alignment | 2
03882..03883 | alignment | 2
03C55..03C57 | alignment | 3
040FB..040FB | alignment | 1
044E1..044E3 | alignment | 3
0451F..0451F | alignment | 1
045EF..045F8 | table | 10
045F9..045FB | alignment | 3
04A0A..04A0B | alignment | 2
04B17..04B17 | alignment | 1
04C7F..04C7F | alignment | 1
04D51..04D53 | alignment | 3
04EEE..04EEF | alignment | 2
0508A..0508B | alignment | 2
0522F..0522F | alignment | 1
0527A..0527B | alignment | 2
052AA..052AB | alignment | 2
053B1..053B3 | alignment | 3
053E2..053E3 | alignment | 2
05451..05453 | alignment | 3
0550B..0550B | alignment | 1
055DB..055DB | alignment | 1
0577D..0577F | alignment | 3
057DB..057DB | alignment | 1
05879..0587F | alignment | 7
05D8F..05D8F | alignment | 1
05F4A..05F4B | alignment | 2
061BF..061BF | alignment | 1
061CE..061CF | alignment | 2
061FD..061FF | alignment | 3
0626F..0626F | alignment | 1
06303..06303 | alignment | 1
06422..06423 | alignment | 2
064A3..064AF | alignment | 13
064D9..064DB | alignment | 3
06616..06617 | alignment | 2
066BD..066BF | alignment | 3
06852..06853 | alignment | 2
06A03..06A03 | noncode | 1
06BBD..06BBF | noncode | 3
06C43..06C43 | noncode | 1
06C5E..06C5F | noncode | 2
06C95..06C97 | noncode | 3
06CFD..06CFF | noncode | 3
06D51..06D53 | noncode | 3
06DE6..06DE7 | noncode | 2
06E4F..06E4F | noncode | 1
06EAD..06EAF | noncode | 3
06F05..06F0F | noncode | 11
06F72..06F73 | noncode | 2
06FF2..06FF3 | noncode | 2
0705B..0705B | noncode | 1
07099..0709B | noncode | 3
0713B..0713B | noncode | 1
07182..07183 | noncode | 2
071B9..071BB | noncode | 3
0723D..0723F | alignment | 3
072C9..072CF | noncode | 7
072DF..072DF | noncode | 1
0735F..0735F | noncode | 1
07469..0746B | noncode | 3
074CE..074CF | noncode | 2
0756A..0756B | noncode | 2
075ED..075EF | noncode | 3
0766A..0766B | alignment | 2
0766C..0767B | table | 16
0782F..0782F | alignment | 1
07843..07843 | alignment | 1
07942..07943 | alignment | 2
079C6..079C7 | alignment | 2
07A61..07A63 | alignment | 3
07AB1..07AB3 | alignment | 3
07B73..07B73 | alignment | 1
07C09..07C0B | alignment | 3
07D67..07D67 | noncode | 1
07D7F..07D7F | alignment | 1
07E03..07E03 | alignment | 1
07E43..07E43 | alignment | 1
07E6B..07E6B | alignment | 1
07F41..07F4F | alignment | 15
07F8A..07F8B | noncode | 2
07FA5..07FA7 | alignment | 3
08029..0802B | alignment | 3
0808A..0808B | alignment | 2
08277..08277 | alignment | 1
08289..0828B | alignment | 3
082A1..082A3 | alignment | 3
082BD..082BF | alignment | 3
08301..08303 | alignment | 3
08421..08423 | alignment | 3
08483..08483 | alignment | 1
08492..0849F | alignment | 14
084C2..084CF | alignment | 14
084DF..084DF | alignment | 1
084F7..084F7 | alignment | 1
085A7..085A7 | alignment | 1
085D3..085D3 | alignment | 1
0864B..0864B | alignment | 1
086C3..086C3 | alignment | 1
08717..08717 | alignment | 1
0873F..0873F | alignment | 1
08787..08787 | alignment | 1
087D5..087D7 | alignment | 3
088D5..088D7 | alignment | 3
088F7..088F7 | alignment | 1
08917..08917 | alignment | 1
0893F..0893F | alignment | 1
0894F..0894F | alignment | 1
0895F..0895F | alignment | 1
08976..08977 | alignment | 2
08A01..08A03 | alignment | 3
08B93..08B93 | alignment | 1
08BE3..08BE3 | alignment | 1
08C3D..08C3F | alignment | 3
08DED..08DEF | alignment | 3
08E06..08E07 | alignment | 2
08E20..08E3B | table | 28
091BC..091D7 | table | 28
09377..09377 | alignment | 1
0943E..0943F | alignment | 2
0951D..0951F | alignment | 3
095BB..095BB | alignment | 1
09777..09777 | alignment | 1
097E5..097E7 | alignment | 3
09811..09813 | alignment | 3
09871..09873 | alignment | 3
09AB2..09AB3 | alignment | 2
09AC2..09AC3 | alignment | 2
09AD2..09AD3 | alignment | 2
09BC2..09BC3 | alignment | 2
09E0F..09E0F | alignment | 1
09E12..09E13 | alignment | 2
09EA3..09EA6 | table | 4
0A2AE..0A2AF | alignment | 2
0A359..0A35B | alignment | 3
0A36A..0A36B | alignment | 2
0A38D..0A38F | alignment | 3
0A3E5..0A3E7 | alignment | 3
0A473..0A473 | alignment | 1
0A4E7..0A4E7 | alignment | 1
0A555..0A557 | alignment | 3
0AE5A..0AE5B | alignment | 2
0AEA6..0AEA7 | alignment | 2
0AF05..0AF07 | alignment | 3
0AF4B..0AF4B | alignment | 1
0AF63..0AF63 | alignment | 1
0B055..0B05F | alignment | 11
0B0F6..0B0F7 | alignment | 2
0B4D2..0B4D3 | alignment | 2
0B51B..0B51B | alignment | 1
0B596..0B597 | alignment | 2
0B5E1..0B5E3 | alignment | 3
0B6BE..0B6BF | alignment | 2
0B6EB..0B6EB | alignment | 1
0B75B..0B75B | alignment | 1
0B76B..0B76B | alignment | 1
0B821..0B823 | alignment | 3
0B8F3..0B8F3 | alignment | 1
0BA01..0BA03 | alignment | 3
0BA97..0BA97 | alignment | 1
0BBE9..0BBEB | alignment | 3
0BC1A..0BC1B | alignment | 2
0BCB3..0BCB3 | noncode | 1
0BD83..0BD83 | noncode | 1
0BE26..0BE27 | noncode | 2
0BF35..0BF37 | noncode | 3
0BFDF..0BFDF | noncode | 1
0C079..0C07B | noncode | 3
0C10F..0C10F | noncode | 1
0C17B..0C17B | noncode | 1
0C1AF..0C1AF | noncode | 1
0C227..0C227 | noncode | 1
0C2C3..0C2CF | noncode | 13
0C49D..0C49F | noncode | 3
0C57B..0C57B | noncode | 1
0C62A..0C62B | noncode | 2
0C64D..0C64F | noncode | 3
0C65F..0C65F | noncode | 1
0C68E..0C68F | noncode | 2
0C6EE..0C6EF | noncode | 2
0C6F7..0C6F7 | noncode | 1
0C79F..0C79F | alignment | 1
0C8C1..0C8C3 | alignment | 3
0C9D2..0C9D3 | alignment | 2
0CACA..0CACB | alignment | 2
0CBD1..0CBD3 | alignment | 3
0CEE9..0CEEF | alignment | 7
0CF41..0CF43 | alignment | 3
0D179..0D17B | alignment | 3
0D27D..0D27F | alignment | 3
0DA45..0DA47 | alignment | 3
0DE6F..0DE6F | alignment | 1
0DEA1..0DEA6 | alignment | 6
0E187..0E18F | alignment | 9
0E1F1..0E1F3 | alignment | 3
0E207..0E207 | alignment | 1
0E269..0E26B | alignment | 3
0E27F..0E27F | alignment | 1
0EDD3..0EDDF | alignment | 13
0FCE7..0FCE7 | alignment | 1
10436..10437 | alignment | 2
104DE..104DF | alignment | 2
10C6D..10C6E | alignment | 2
<!-- RESIDUAL-RANGES-END -->

## Embedded table ranges

The table category consists of the following already-established data islands:

- `01F00..01F23`: nine-entry code-address jump table.
- `045EF..045F8`: ten-byte local lookup table (`678456789:`).
- `0766C..0767B`: four-entry CS jump table; `0766A..0766B` immediately before it is alignment.
- `08E20..08E3B`: address table used by the asset dispatcher.
- `091BC..091D7`: address table.
- `09EA3..09EA6`: preserved four-byte prefix of the batch-12 lookup data island; `09EA7` and following bytes were subsequently verified as code and are not classified here.

`noncode` rows are arithmetic-closed batch exclusions whose exact non-code status is proven but whose narrower padding-vs-data subtype is not explicit in preserved evidence. All `alignment` rows are NOP/LEA/MOV-self padding or explicitly documented zero/data-alignment bytes.

## Accounting rule

Do not add these 677 bytes to `kVerifiedTranslatedBytes`: they are classified residual bytes, not translated application instructions. Do not reduce the denominator merely to improve the displayed percentage. Completion requires the remaining 73 bytes to be individually classified or translated and all accounting reconciled without overlap or double-counting.
