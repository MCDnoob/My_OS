# handler
.text
.globl __alltraps
# exceptions/interrupts

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp __alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp __alltraps

TRAPHANDLER_NOEC(handler0, 0)
TRAPHANDLER_NOEC(handler1, 1)
TRAPHANDLER_NOEC(handler2, 2)
TRAPHANDLER_NOEC(handler3, 3)
TRAPHANDLER_NOEC(handler4, 4)
TRAPHANDLER_NOEC(handler5, 5)
TRAPHANDLER_NOEC(handler6, 6)
TRAPHANDLER_NOEC(handler7, 7)
TRAPHANDLER(handler8, 8)
TRAPHANDLER_NOEC(handler9, 9)
TRAPHANDLER(handler10, 10)
TRAPHANDLER(handler11, 11)
TRAPHANDLER(handler12, 12)
TRAPHANDLER(handler13, 13)
TRAPHANDLER(handler14, 14)
TRAPHANDLER_NOEC(handler15, 15)
TRAPHANDLER_NOEC(handler16, 16)
TRAPHANDLER_NOEC(handler17, 17)
TRAPHANDLER_NOEC(handler18, 18)
TRAPHANDLER_NOEC(handler19, 19)
TRAPHANDLER_NOEC(handler20, 20)
TRAPHANDLER_NOEC(handler21, 21)
TRAPHANDLER_NOEC(handler22, 22)
TRAPHANDLER_NOEC(handler23, 23)
TRAPHANDLER_NOEC(handler24, 24)
TRAPHANDLER_NOEC(handler25, 25)
TRAPHANDLER_NOEC(handler26, 26)
TRAPHANDLER_NOEC(handler27, 27)
TRAPHANDLER_NOEC(handler28, 28)
TRAPHANDLER_NOEC(handler29, 29)
TRAPHANDLER_NOEC(handler30, 30)
TRAPHANDLER_NOEC(handler31, 31)
TRAPHANDLER_NOEC(handler32, 32)
TRAPHANDLER_NOEC(handler33, 33)
TRAPHANDLER_NOEC(handler34, 34)
TRAPHANDLER_NOEC(handler35, 35)
TRAPHANDLER_NOEC(handler36, 36)
TRAPHANDLER_NOEC(handler37, 37)
TRAPHANDLER_NOEC(handler38, 38)
TRAPHANDLER_NOEC(handler39, 39)
TRAPHANDLER_NOEC(handler40, 40)
TRAPHANDLER_NOEC(handler41, 41)
TRAPHANDLER_NOEC(handler42, 42)
TRAPHANDLER_NOEC(handler43, 43)
TRAPHANDLER_NOEC(handler44, 44)
TRAPHANDLER_NOEC(handler45, 45)
TRAPHANDLER_NOEC(handler46, 46)
TRAPHANDLER_NOEC(handler47, 47)
TRAPHANDLER_NOEC(handler48, 48)
TRAPHANDLER_NOEC(handler49, 49)
TRAPHANDLER_NOEC(handler50, 50)
TRAPHANDLER_NOEC(handler51, 51)
TRAPHANDLER_NOEC(handler52, 52)
TRAPHANDLER_NOEC(handler53, 53)
TRAPHANDLER_NOEC(handler54, 54)
TRAPHANDLER_NOEC(handler55, 55)
TRAPHANDLER_NOEC(handler56, 56)
TRAPHANDLER_NOEC(handler57, 57)
TRAPHANDLER_NOEC(handler58, 58)
TRAPHANDLER_NOEC(handler59, 59)
TRAPHANDLER_NOEC(handler60, 60)
TRAPHANDLER_NOEC(handler61, 61)
TRAPHANDLER_NOEC(handler62, 62)
TRAPHANDLER_NOEC(handler63, 63)
TRAPHANDLER_NOEC(handler64, 64)
TRAPHANDLER_NOEC(handler65, 65)
TRAPHANDLER_NOEC(handler66, 66)
TRAPHANDLER_NOEC(handler67, 67)
TRAPHANDLER_NOEC(handler68, 68)
TRAPHANDLER_NOEC(handler69, 69)
TRAPHANDLER_NOEC(handler70, 70)
TRAPHANDLER_NOEC(handler71, 71)
TRAPHANDLER_NOEC(handler72, 72)
TRAPHANDLER_NOEC(handler73, 73)
TRAPHANDLER_NOEC(handler74, 74)
TRAPHANDLER_NOEC(handler75, 75)
TRAPHANDLER_NOEC(handler76, 76)
TRAPHANDLER_NOEC(handler77, 77)
TRAPHANDLER_NOEC(handler78, 78)
TRAPHANDLER_NOEC(handler79, 79)
TRAPHANDLER_NOEC(handler80, 80)
TRAPHANDLER_NOEC(handler81, 81)
TRAPHANDLER_NOEC(handler82, 82)
TRAPHANDLER_NOEC(handler83, 83)
TRAPHANDLER_NOEC(handler84, 84)
TRAPHANDLER_NOEC(handler85, 85)
TRAPHANDLER_NOEC(handler86, 86)
TRAPHANDLER_NOEC(handler87, 87)
TRAPHANDLER_NOEC(handler88, 88)
TRAPHANDLER_NOEC(handler89, 89)
TRAPHANDLER_NOEC(handler90, 90)
TRAPHANDLER_NOEC(handler91, 91)
TRAPHANDLER_NOEC(handler92, 92)
TRAPHANDLER_NOEC(handler93, 93)
TRAPHANDLER_NOEC(handler94, 94)
TRAPHANDLER_NOEC(handler95, 95)
TRAPHANDLER_NOEC(handler96, 96)
TRAPHANDLER_NOEC(handler97, 97)
TRAPHANDLER_NOEC(handler98, 98)
TRAPHANDLER_NOEC(handler99, 99)
TRAPHANDLER_NOEC(handler100, 100)
TRAPHANDLER_NOEC(handler101, 101)
TRAPHANDLER_NOEC(handler102, 102)
TRAPHANDLER_NOEC(handler103, 103)
TRAPHANDLER_NOEC(handler104, 104)
TRAPHANDLER_NOEC(handler105, 105)
TRAPHANDLER_NOEC(handler106, 106)
TRAPHANDLER_NOEC(handler107, 107)
TRAPHANDLER_NOEC(handler108, 108)
TRAPHANDLER_NOEC(handler109, 109)
TRAPHANDLER_NOEC(handler110, 110)
TRAPHANDLER_NOEC(handler111, 111)
TRAPHANDLER_NOEC(handler112, 112)
TRAPHANDLER_NOEC(handler113, 113)
TRAPHANDLER_NOEC(handler114, 114)
TRAPHANDLER_NOEC(handler115, 115)
TRAPHANDLER_NOEC(handler116, 116)
TRAPHANDLER_NOEC(handler117, 117)
TRAPHANDLER_NOEC(handler118, 118)
TRAPHANDLER_NOEC(handler119, 119)
TRAPHANDLER_NOEC(handler120, 120)
TRAPHANDLER_NOEC(handler121, 121)
TRAPHANDLER_NOEC(handler122, 122)
TRAPHANDLER_NOEC(handler123, 123)
TRAPHANDLER_NOEC(handler124, 124)
TRAPHANDLER_NOEC(handler125, 125)
TRAPHANDLER_NOEC(handler126, 126)
TRAPHANDLER_NOEC(handler127, 127)
TRAPHANDLER_NOEC(handler128, 128)
TRAPHANDLER_NOEC(handler129, 129)
TRAPHANDLER_NOEC(handler130, 130)
TRAPHANDLER_NOEC(handler131, 131)
TRAPHANDLER_NOEC(handler132, 132)
TRAPHANDLER_NOEC(handler133, 133)
TRAPHANDLER_NOEC(handler134, 134)
TRAPHANDLER_NOEC(handler135, 135)
TRAPHANDLER_NOEC(handler136, 136)
TRAPHANDLER_NOEC(handler137, 137)
TRAPHANDLER_NOEC(handler138, 138)
TRAPHANDLER_NOEC(handler139, 139)
TRAPHANDLER_NOEC(handler140, 140)
TRAPHANDLER_NOEC(handler141, 141)
TRAPHANDLER_NOEC(handler142, 142)
TRAPHANDLER_NOEC(handler143, 143)
TRAPHANDLER_NOEC(handler144, 144)
TRAPHANDLER_NOEC(handler145, 145)
TRAPHANDLER_NOEC(handler146, 146)
TRAPHANDLER_NOEC(handler147, 147)
TRAPHANDLER_NOEC(handler148, 148)
TRAPHANDLER_NOEC(handler149, 149)
TRAPHANDLER_NOEC(handler150, 150)
TRAPHANDLER_NOEC(handler151, 151)
TRAPHANDLER_NOEC(handler152, 152)
TRAPHANDLER_NOEC(handler153, 153)
TRAPHANDLER_NOEC(handler154, 154)
TRAPHANDLER_NOEC(handler155, 155)
TRAPHANDLER_NOEC(handler156, 156)
TRAPHANDLER_NOEC(handler157, 157)
TRAPHANDLER_NOEC(handler158, 158)
TRAPHANDLER_NOEC(handler159, 159)
TRAPHANDLER_NOEC(handler160, 160)
TRAPHANDLER_NOEC(handler161, 161)
TRAPHANDLER_NOEC(handler162, 162)
TRAPHANDLER_NOEC(handler163, 163)
TRAPHANDLER_NOEC(handler164, 164)
TRAPHANDLER_NOEC(handler165, 165)
TRAPHANDLER_NOEC(handler166, 166)
TRAPHANDLER_NOEC(handler167, 167)
TRAPHANDLER_NOEC(handler168, 168)
TRAPHANDLER_NOEC(handler169, 169)
TRAPHANDLER_NOEC(handler170, 170)
TRAPHANDLER_NOEC(handler171, 171)
TRAPHANDLER_NOEC(handler172, 172)
TRAPHANDLER_NOEC(handler173, 173)
TRAPHANDLER_NOEC(handler174, 174)
TRAPHANDLER_NOEC(handler175, 175)
TRAPHANDLER_NOEC(handler176, 176)
TRAPHANDLER_NOEC(handler177, 177)
TRAPHANDLER_NOEC(handler178, 178)
TRAPHANDLER_NOEC(handler179, 179)
TRAPHANDLER_NOEC(handler180, 180)
TRAPHANDLER_NOEC(handler181, 181)
TRAPHANDLER_NOEC(handler182, 182)
TRAPHANDLER_NOEC(handler183, 183)
TRAPHANDLER_NOEC(handler184, 184)
TRAPHANDLER_NOEC(handler185, 185)
TRAPHANDLER_NOEC(handler186, 186)
TRAPHANDLER_NOEC(handler187, 187)
TRAPHANDLER_NOEC(handler188, 188)
TRAPHANDLER_NOEC(handler189, 189)
TRAPHANDLER_NOEC(handler190, 190)
TRAPHANDLER_NOEC(handler191, 191)
TRAPHANDLER_NOEC(handler192, 192)
TRAPHANDLER_NOEC(handler193, 193)
TRAPHANDLER_NOEC(handler194, 194)
TRAPHANDLER_NOEC(handler195, 195)
TRAPHANDLER_NOEC(handler196, 196)
TRAPHANDLER_NOEC(handler197, 197)
TRAPHANDLER_NOEC(handler198, 198)
TRAPHANDLER_NOEC(handler199, 199)
TRAPHANDLER_NOEC(handler200, 200)
TRAPHANDLER_NOEC(handler201, 201)
TRAPHANDLER_NOEC(handler202, 202)
TRAPHANDLER_NOEC(handler203, 203)
TRAPHANDLER_NOEC(handler204, 204)
TRAPHANDLER_NOEC(handler205, 205)
TRAPHANDLER_NOEC(handler206, 206)
TRAPHANDLER_NOEC(handler207, 207)
TRAPHANDLER_NOEC(handler208, 208)
TRAPHANDLER_NOEC(handler209, 209)
TRAPHANDLER_NOEC(handler210, 210)
TRAPHANDLER_NOEC(handler211, 211)
TRAPHANDLER_NOEC(handler212, 212)
TRAPHANDLER_NOEC(handler213, 213)
TRAPHANDLER_NOEC(handler214, 214)
TRAPHANDLER_NOEC(handler215, 215)
TRAPHANDLER_NOEC(handler216, 216)
TRAPHANDLER_NOEC(handler217, 217)
TRAPHANDLER_NOEC(handler218, 218)
TRAPHANDLER_NOEC(handler219, 219)
TRAPHANDLER_NOEC(handler220, 220)
TRAPHANDLER_NOEC(handler221, 221)
TRAPHANDLER_NOEC(handler222, 222)
TRAPHANDLER_NOEC(handler223, 223)
TRAPHANDLER_NOEC(handler224, 224)
TRAPHANDLER_NOEC(handler225, 225)
TRAPHANDLER_NOEC(handler226, 226)
TRAPHANDLER_NOEC(handler227, 227)
TRAPHANDLER_NOEC(handler228, 228)
TRAPHANDLER_NOEC(handler229, 229)
TRAPHANDLER_NOEC(handler230, 230)
TRAPHANDLER_NOEC(handler231, 231)
TRAPHANDLER_NOEC(handler232, 232)
TRAPHANDLER_NOEC(handler233, 233)
TRAPHANDLER_NOEC(handler234, 234)
TRAPHANDLER_NOEC(handler235, 235)
TRAPHANDLER_NOEC(handler236, 236)
TRAPHANDLER_NOEC(handler237, 237)
TRAPHANDLER_NOEC(handler238, 238)
TRAPHANDLER_NOEC(handler239, 239)
TRAPHANDLER_NOEC(handler240, 240)
TRAPHANDLER_NOEC(handler241, 241)
TRAPHANDLER_NOEC(handler242, 242)
TRAPHANDLER_NOEC(handler243, 243)
TRAPHANDLER_NOEC(handler244, 244)
TRAPHANDLER_NOEC(handler245, 245)
TRAPHANDLER_NOEC(handler246, 246)
TRAPHANDLER_NOEC(handler247, 247)
TRAPHANDLER_NOEC(handler248, 248)
TRAPHANDLER_NOEC(handler249, 249)
TRAPHANDLER_NOEC(handler250, 250)
TRAPHANDLER_NOEC(handler251, 251)
TRAPHANDLER_NOEC(handler252, 252)
TRAPHANDLER_NOEC(handler253, 253)
TRAPHANDLER_NOEC(handler254, 254)
TRAPHANDLER_NOEC(handler255, 255)

.data
.globl __vectors
__vectors:
	.long handler0
	.long handler1
	.long handler2
	.long handler3
	.long handler4
	.long handler5
	.long handler6
	.long handler7
	.long handler8
	.long handler9
	.long handler10
	.long handler11
	.long handler12
	.long handler13
	.long handler14
	.long handler15
	.long handler16
	.long handler17
	.long handler18
	.long handler19
	.long handler20
	.long handler21
	.long handler22
	.long handler23
	.long handler24
	.long handler25
	.long handler26
	.long handler27
	.long handler28
	.long handler29
	.long handler30
	.long handler31
	.long handler32
	.long handler33
	.long handler34
	.long handler35
	.long handler36
	.long handler37
	.long handler38
	.long handler39
	.long handler40
	.long handler41
	.long handler42
	.long handler43
	.long handler44
	.long handler45
	.long handler46
	.long handler47
	.long handler48
	.long handler49
	.long handler50
	.long handler51
	.long handler52
	.long handler53
	.long handler54
	.long handler55
	.long handler56
	.long handler57
	.long handler58
	.long handler59
	.long handler60
	.long handler61
	.long handler62
	.long handler63
	.long handler64
	.long handler65
	.long handler66
	.long handler67
	.long handler68
	.long handler69
	.long handler70
	.long handler71
	.long handler72
	.long handler73
	.long handler74
	.long handler75
	.long handler76
	.long handler77
	.long handler78
	.long handler79
	.long handler80
	.long handler81
	.long handler82
	.long handler83
	.long handler84
	.long handler85
	.long handler86
	.long handler87
	.long handler88
	.long handler89
	.long handler90
	.long handler91
	.long handler92
	.long handler93
	.long handler94
	.long handler95
	.long handler96
	.long handler97
	.long handler98
	.long handler99
	.long handler100
	.long handler101
	.long handler102
	.long handler103
	.long handler104
	.long handler105
	.long handler106
	.long handler107
	.long handler108
	.long handler109
	.long handler110
	.long handler111
	.long handler112
	.long handler113
	.long handler114
	.long handler115
	.long handler116
	.long handler117
	.long handler118
	.long handler119
	.long handler120
	.long handler121
	.long handler122
	.long handler123
	.long handler124
	.long handler125
	.long handler126
	.long handler127
	.long handler128
	.long handler129
	.long handler130
	.long handler131
	.long handler132
	.long handler133
	.long handler134
	.long handler135
	.long handler136
	.long handler137
	.long handler138
	.long handler139
	.long handler140
	.long handler141
	.long handler142
	.long handler143
	.long handler144
	.long handler145
	.long handler146
	.long handler147
	.long handler148
	.long handler149
	.long handler150
	.long handler151
	.long handler152
	.long handler153
	.long handler154
	.long handler155
	.long handler156
	.long handler157
	.long handler158
	.long handler159
	.long handler160
	.long handler161
	.long handler162
	.long handler163
	.long handler164
	.long handler165
	.long handler166
	.long handler167
	.long handler168
	.long handler169
	.long handler170
	.long handler171
	.long handler172
	.long handler173
	.long handler174
	.long handler175
	.long handler176
	.long handler177
	.long handler178
	.long handler179
	.long handler180
	.long handler181
	.long handler182
	.long handler183
	.long handler184
	.long handler185
	.long handler186
	.long handler187
	.long handler188
	.long handler189
	.long handler190
	.long handler191
	.long handler192
	.long handler193
	.long handler194
	.long handler195
	.long handler196
	.long handler197
	.long handler198
	.long handler199
	.long handler200
	.long handler201
	.long handler202
	.long handler203
	.long handler204
	.long handler205
	.long handler206
	.long handler207
	.long handler208
	.long handler209
	.long handler210
	.long handler211
	.long handler212
	.long handler213
	.long handler214
	.long handler215
	.long handler216
	.long handler217
	.long handler218
	.long handler219
	.long handler220
	.long handler221
	.long handler222
	.long handler223
	.long handler224
	.long handler225
	.long handler226
	.long handler227
	.long handler228
	.long handler229
	.long handler230
	.long handler231
	.long handler232
	.long handler233
	.long handler234
	.long handler235
	.long handler236
	.long handler237
	.long handler238
	.long handler239
	.long handler240
	.long handler241
	.long handler242
	.long handler243
	.long handler244
	.long handler245
	.long handler246
	.long handler247
	.long handler248
	.long handler249
	.long handler250
	.long handler251
	.long handler252
	.long handler253
	.long handler254
	.long handler255
