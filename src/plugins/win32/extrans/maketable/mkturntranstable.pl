#!/usr/local/bin/perl

;# 'turn' �g�����W�V�����p�̕ό`�p�e�[�u�����쐬����

;# 64x64 �̐����`��H�`�ɕό`����ϊ����l����
;#                        �P�P�P�P�P�P�P�P�P�P�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�R�R
;#    �O�P�Q�R�S�T�U�V�W�X�O�P�Q�R�S�T�U�V�W�X�O�P�Q�R�S�T�U�V�W�X�O�P
;#  0 ����������������������������������������������������������������
;#  1 ���_�����E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�^��
;#  2 ���E�_�E�������E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�^�E��
;#  3 �����E�_�E�E�E�����E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�^�E�E��
;#  4 �����E�E�_�E�E�E�E�������E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�^�E�E�E��
;#  5 �����E�E�E�_�E�E�E�E�E�E�����E�E�E�E�E�E�E�E�E�E�E�E�^�E�E�E�E��
;#  6 �����E�E�E�E�_�E�E�E�E�E�E�E�����E�E�E�E�E�E�E�E�E�^�E�E�E�E�E��
;#  7 �����E�E�E�E�E�_�E�E�E�E�E�E�E�E�������E�E�E�E�E�^�E�E�E�E�E�E��
;#  8 ���E���E�E�E�E�E�_�E�E�E�E�E�E�E�E�E�E�����E�E�^�E�E�E�E�E�E�E��
;#  9 ���E���E�E�E�E�E�E�_�E�E�E�E�E�E�E�E�E�E�E�������E�E�E���E�E�E��
;# 10 ���E���E�E�E�E�E�E�E�_�E�E�E�E�E�E�E�E�E�E�^�E�E�����E���E�E�E��
;# 11 ���E���E�E�E�E�E�E�E�E�_�E�E�E�E�E�E�E�E�^�E�E�E�E�E�����E�E�E��
;# 12 ���E���E�E�E�E�E�E�E�E�E�_�E�E�E�E�E�E�^�E�E�E�E�E�E�E���E�E�E��
;# 13 ���E�E���E�E�E�E�E�E�E�E�E�_�E�E�E�E�^�E�E�E�E�E�E�E�E���E�E�E��
;# 14 ���E�E���E�E�E�E�E�E�E�E�E�E�_�E�E�^�E�E�E�E�E�E�E�E�E�E���E�E��
;# 15 ���E�E���E�E�E�E�E�E�E�E�E�E�E�_�^�E�E�E�E�E�E�E�E�E�E�E���E�E��
;# 16 ���E�E���E�E�E�E�E�E�E�E�E�E�E�^�_�E�E�E�E�E�E�E�E�E�E�E���E�E��
;# 17 ���E�E���E�E�E�E�E�E�E�E�E�E�^�E�E�_�E�E�E�E�E�E�E�E�E�E���E�E��
;# 18 ���E�E�E���E�E�E�E�E�E�E�E�^�E�E�E�E�_�E�E�E�E�E�E�E�E�E���E�E��
;# 19 ���E�E�E���E�E�E�E�E�E�E�^�E�E�E�E�E�E�_�E�E�E�E�E�E�E�E�E���E��
;# 20 ���E�E�E�����E�E�E�E�E�^�E�E�E�E�E�E�E�E�_�E�E�E�E�E�E�E�E���E��
;# 21 ���E�E�E���E�����E�E�^�E�E�E�E�E�E�E�E�E�E�_�E�E�E�E�E�E�E���E��
;# 22 ���E�E�E���E�E�E�������E�E�E�E�E�E�E�E�E�E�E�_�E�E�E�E�E�E���E��
;# 23 ���E�E�E�E�E�E�E�^�E�E�����E�E�E�E�E�E�E�E�E�E�_�E�E�E�E�E���E��
;# 24 ���E�E�E�E�E�E�^�E�E�E�E�E�������E�E�E�E�E�E�E�E�_�E�E�E�E�E����
;# 25 ���E�E�E�E�E�^�E�E�E�E�E�E�E�E�E�����E�E�E�E�E�E�E�_�E�E�E�E����
;# 26 ���E�E�E�E�^�E�E�E�E�E�E�E�E�E�E�E�E�����E�E�E�E�E�E�_�E�E�E����
;# 27 ���E�E�E�^�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�������E�E�E�E�_�E�E����
;# 28 ���E�E�^�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�����E�E�E�_�E����
;# 29 ���E�^�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�������E�_�E��
;# 30 ���^�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�E�����_��
;# 31 ����������������������������������������������������������������

;# ( ���̐}�� 32x32 )

;# �����Ŏ����������A���Ŏ������H�`�ɕό`�����B
;# �ϊ��̓X�L�������C�� ( y ���W ) ���Ƃɋ��߂�B

;# cpp �擪�����o��
open FH, ">../turntrans_table.cpp";

print FH <<EOF;
/* 'turn' �g�����W�V�����p�ό`�ϊ��e�[�u�� */
/* ���̃t�@�C���� mkturntranstable.pl �ɂ��쐬����܂��� */

#include "turntrans_table.h"

const tTurnTransParams TurnTransParams[64][64] =
{
	/* { start, len, sx, sy, ex, ey, stepx, stepy } */

EOF


;# n = 0 �̃_�~�[�f�[�^�o��

print FH "\t/* n = 0 (not used) */\n";
print FH "\t{\n";

for($y = 0; $y < 64; $y++)
{
	print FH ",\n" if $y != 0;
	printf FH "\t\t/* y = %2d */ {0, 0, 0, 0, 0, 0, 0, 0}", $y;
}
print FH "\n";
print FH "\t},\n";

for($n = 1; $n <= 62; $n++)
{
	;# $y (�X�L�������C��)
	@starts = ();
	@ends = ();
	@len = ();
	@sx = ();
	@sy = ();
	@ex = ();
	@ey = ();
	@stepx = ();
	@stepy = ();

	;# ���_�A���_�̌v�Z�Ɋւ��Ă͂��܂茵���Ȏ���p���Ȃ�
	;# ( �����Ɍv�Z����ƕ��C�ł͂ݏo�邱�ƂɂȂ� )
	;# �܂�������Ƃ��炢�䂪��ł��Ă��������Ⴂ����C�ɂȂ��͂�

	;# ���_���v�Z
	if($n < 32)
	{
		$p = int($n * $n / 31);
		$d = int(sin($p * 3.14159265358979 / 64)*4);
		$a_x = $p - $d;
		$a_y = 63 - $p - $d;
	}
	else
	{
		$p = 63 - int((63 - $n) * (63 - $n) / 31);
		$d = int(sin((63-$p) * 3.14159265358979 / 64)*4);
		$a_x = 63 - $p + $d;
		$a_y = $p + $d;
	}

	;# ���_���v�Z
	$b_x = 63 - $a_x;
	$b_y = 63 - $a_y;

	print FH "\t/* n = $n */\n";
	printf FH "\t/* �� = (%d, %d)  �� = (%d, %d) */\n", $a_x, $a_y, $b_x, $b_y;


	for($y = 0; $y < 64; $y ++)
	{
		if($y <= $a_y)
		{
			;# (0,0) - (���_) ������� x ���W $l �����߂�
			if($a_y == 0)
			{
				$l = 0;
			}
			else
			{
				$l = int($a_x * $y / $a_y);
			}
		}
		else
		{
			;# (���_) - (63,63) ������� x ���W $l �����߂�
			if($a_y != 63)
			{
				$l = $a_x + int((63 - $a_x) * ($y - $a_y) / (63 - $a_y));
			}
			else
			{
				$l = 63;
			}
		}

		if($y <= $b_y)
		{
			;# (0,0) - (���_) ������� x ���W $r �����߂�
			if($b_y == 0)
			{
				$r = 63;
			}
			else
			{
				$r = int($b_x * $y / $b_y);
			}
		}
		else
		{
			;# (���_) - (63,63) ������� x ���W $r �����߂�
			if($b_y != 63)
			{
				$r = $b_x + int((63 - $b_x) * ($y - $b_y) / (63 - $b_y));
			}
			else
			{
				$r = 63;
			}
		}

		if($l > $r) { $r = $l; } ;# �덷�␳

		push(@starts, $l);
		push(@ends, $r);

		;# ���� $len �����߂�
		$len = $r - $l + 1;

		push(@len, $len);

		;# �ϊ��������`�̊J�n�ʒu $sx, $sy �����߂�
		if($y <= $a_y)
		{
			$sx = 0;
			$sy = int(63 * $y / $a_y) if $a_y;
			$sy = 0 if !$a_y;
		}
		else
		{
			$sx = int(63 * ($y - $a_y) / (63 - $a_y));
			$sy = 63;
		}

		push(@sx, $sx << 16);
		push(@sy, $sy << 16);

		;# �ϊ��������`�̏I���ʒu $ex, $ey �����߂�
		if($y <= $b_y)
		{
			$ex = int(63 * $y / $b_y) if $b_y;
			$ex = 63 if !$b_y;
			$ey = 0;
		}
		else
		{
			$ex = 63;
			$ey = int(63 * ($y - $b_y) / (63 - $b_y));
		}

		push(@ex, $ex << 16);
		push(@ey, $ey << 16);

		;# �ϊ��������`�𑖍����邽�߂� x, y �̃X�e�b�v ��
		;# 16bit�ڂɏ����_�����Œ菬���_���ŋ��߂�
		if($len >= 2)
		{
			$stepx = int(($ex - $sx) / ($len-1) * 65536);
			$stepy = int(($ey - $sy) / ($len-1) * 65536);
		}
		else
		{
			$stepx = 0;
			$stepy = 0;
		}

		push(@stepx, $stepx);
		push(@stepy, $stepy);
	}

	;# �f�[�^�̏����o��
	print FH "\t{\n";

	for($y = 0; $y < 64; $y++)
	{
		print FH ",\n" if $y != 0;
		printf FH "\t\t/* y = %2d */ /* l = %3d, r = %2d */ {%2d, %2d, %7d, %7d, %7d, %7d, %8d, %8d}",
			$y, $starts[$y], $ends[$y],
			$starts[$y], $len[$y],
			$sx[$y], $sy[$y], $ex[$y], $ey[$y],
			$stepx[$y], $stepy[$y];
	}

	print FH "\n";
	print FH "\t},\n";
}


;# n = 63 �̃_�~�[�f�[�^�o��

print FH "\t/* n = 63 (not used) */\n";
print FH "\t{\n";

for($y = 0; $y < 64; $y++)
{
	print FH ",\n" if $y != 0;
	printf FH "\t\t/* y = %2d */ {0, 0, 0, 0, 0, 0, 0, 0}", $y;
}
print FH "\n";
print FH "\t}\n";


print FH "};\n";

;# �w�b�_�o��

open FH, ">../turntrans_table.h";

print FH <<EOF;
/* 'turn' �g�����W�V�����p�ό`�ϊ��e�[�u�� */
/* ���̃t�@�C���� mkturntranstable.pl �ɂ��쐬����܂��� */

#ifndef turntrans_table_H
#define turntrans_table_H

#pragma pack(push, 4)
struct tTurnTransParams
{
	int start;
	int len;
	int sx; // source start x ( * 65536 )
	int sy; // source start y ( * 65536 )
	int ex; // source end x ( * 65536 )
	int ey; // source end y ( * 65536 )
	int stepx; // source step x ( * 65536 )
	int stepy; // source step y ( * 65536 )
};
#pragma pack(pop)

extern const tTurnTransParams TurnTransParams[64][64];


#endif

EOF



