;    1: PROGRAM simple (output);
	DOSSEG
	.MODEL  small
	.STACK  1024

	.CODE

	PUBLIC	_pascal_main
	INCLUDE	pasextrn.inc

$STATIC_LINK		EQU	<WORD PTR [bp+4]>
$RETURN_VALUE		EQU	<WORD PTR [bp-4]>
$HIGH_RETURN_VALUE	EQU	<WORD PTR [bp-2]>

;    2: 
;    3:     VAR
;    4: 	n : integer;
;    5: 	x : real;
;    6: 
;    7:     PROCEDURE proc (i : integer; VAR j : integer);
;    8: 
;    9: 	FUNCTION func (y : real) : real;

i_003	EQU	<WORD PTR [bp+14]>
j_004	EQU	<WORD PTR [bp+6]>
;   10: 
;   11: 	    BEGIN {func}

y_005	EQU	<WORD PTR [bp+6]>

func_000	PROC

	push	bp
	mov	bp,sp
	sub	sp,4
;   12: 		j := 5;
	mov	bx,bp
	mov	bp,$STATIC_LINK
	mov	ax,WORD PTR j_004
	mov	bp,bx
	push	ax
	mov	ax,5
	pop	bx
	mov	WORD PTR [bx],ax
;   13: 		func := i + y + 0.5;
	lea	ax,$RETURN_VALUE
	push	ax
	mov	bx,bp
	mov	bp,$STATIC_LINK
	mov	ax,WORD PTR i_003
	mov	bp,bx
	push	ax
	mov	ax,WORD PTR y_005
	mov	dx,WORD PTR y_005+2
	push	dx
	push	ax
	pop	ax
	pop	dx
	pop	bx
	push	dx
	push	ax
	push	bx
	call	_float_convert
	add	sp,2
	pop	bx
	pop	cx
	push	dx
	push	ax
	push	cx
	push	bx
	call	_float_add
	add	sp,8
	push	dx
	push	ax
	mov	ax,WORD PTR $F_006
	mov	dx,WORD PTR $F_006+2
	push	dx
	push	ax
	call	_float_add
	add	sp,8
	pop	bx
	mov	WORD PTR [bx],ax
	mov	WORD PTR [bx+2],dx
;   14: 	    END {func};
	mov	ax,$RETURN_VALUE
	mov	dx,$HIGH_RETURN_VALUE
	mov	sp,bp
	pop	bp
	ret	6

func_000	ENDP
;   15: 
;   16: 	BEGIN {proc}

proc_000	PROC

	push	bp
	mov	bp,sp
;   17: 	    j := i DIV 2;
	mov	ax,WORD PTR j_004
	push	ax
	mov	ax,WORD PTR i_003
	push	ax
	mov	ax,2
	mov	cx,ax
	pop	ax
	sub	dx,dx
	idiv	cx
	pop	bx
	mov	WORD PTR [bx],ax
;   18: 	    writeln('In proc, the value of j is', j:3);
	lea	ax,WORD PTR $S_007
	push	ax
	mov	ax,0
	push	ax
	mov	ax,26
	push	ax
	call	_write_string
	add	sp,6
	mov	bx,WORD PTR j_004
	mov	ax,WORD PTR [bx]
	push	ax
	mov	ax,3
	push	ax
	call	_write_integer
	add	sp,4
	call	_write_line
;   19: 	    x := func(3.14);
	mov	ax,WORD PTR $F_008
	mov	dx,WORD PTR $F_008+2
	push	dx
	push	ax
	push	bp
	call	func_000
	mov	WORD PTR x_002,ax
	mov	WORD PTR x_002+2,dx
;   20: 	    writeln('In proc, the value of j is', j:3);
	lea	ax,WORD PTR $S_007
	push	ax
	mov	ax,0
	push	ax
	mov	ax,26
	push	ax
	call	_write_string
	add	sp,6
	mov	bx,WORD PTR j_004
	mov	ax,WORD PTR [bx]
	push	ax
	mov	ax,3
	push	ax
	call	_write_integer
	add	sp,4
	call	_write_line
;   21: 	END {proc};
	mov	sp,bp
	pop	bp
	ret	14

proc_000	ENDP
;   22: 
;   23:     BEGIN {simple}

_pascal_main	PROC

	push	bp
	mov	bp,sp
;   24: 	n := 1;
	mov	ax,1
	mov	WORD PTR n_001,ax
;   25: 	writeln('In simple, the value of n is', n:3);
	lea	ax,WORD PTR $S_009
	push	ax
	mov	ax,0
	push	ax
	mov	ax,28
	push	ax
	call	_write_string
	add	sp,6
	mov	ax,WORD PTR n_001
	push	ax
	mov	ax,3
	push	ax
	call	_write_integer
	add	sp,4
	call	_write_line
;   26: 	proc(7, n);
	mov	ax,7
	push	ax
	lea	ax,WORD PTR n_001
	push	ax
	push	bp
	call	proc_000
;   27: 	writeln('In simple, the value of n is', n:3,
	lea	ax,WORD PTR $S_009
	push	ax
	mov	ax,0
	push	ax
	mov	ax,28
	push	ax
	call	_write_string
	add	sp,6
	mov	ax,WORD PTR n_001
	push	ax
	mov	ax,3
	push	ax
	call	_write_integer
	add	sp,4
;   28: 		' and the value of x is', x:8:4);
	lea	ax,WORD PTR $S_010
	push	ax
	mov	ax,0
	push	ax
	mov	ax,22
	push	ax
	call	_write_string
	add	sp,6
	mov	ax,WORD PTR x_002
	mov	dx,WORD PTR x_002+2
	push	dx
	push	ax
	mov	ax,8
	push	ax
	mov	ax,4
	push	ax
	call	_write_real
	add	sp,8
	call	_write_line
;   29:     END {simple}.
;   30: 
;   31: 
;   32: 
	pop	bp
	ret

_pascal_main	ENDP

	.DATA

n_001	DW	0
x_002	DD	0.0
$F_008	DD	3.140000e+00
$F_006	DD	5.000000e-01
$S_010	DB	" and the value of x is"
$S_009	DB	"In simple, the value of n is"
$S_007	DB	"In proc, the value of j is"

	END
