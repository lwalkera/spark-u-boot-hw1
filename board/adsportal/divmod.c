/*
 * This file is part of GNU CC.
 *
 * GNU CC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 *
 * GNU CC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with GNU CC; see the file COPYING.  If not, write
 * to the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


int __divsi3 (int a, int b)
{
	int neg = 0;
	int res;

	if (a < 0) {
		a = -a;
		neg = !neg;
	}

	if (b < 0) {
		b = -b;
		neg = !neg;
	}

	res = __udivsi3 (a, b);

	if (neg)
		res = -res;

	return res;
}


int __modsi3 (int a, int b)
{
	int neg = 0;
	int res;

	if (a < 0) {
		a = -a;
		neg = 1;
	}

	if (b < 0)
		b = -b;

	res = __umodsi3 (a, b);

	if (neg)
		res = -res;

	return res;
}

