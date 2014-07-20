/*
* ���ƣ������½�����DFP��ţ�ٷ�
* ���ԣ�C����
* ���ߣ�keke2014
* ʱ�䣺2013.01.10
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define pi 3.1415926

/* ����1��ָ�����ֵ */
double fun1(double **vars, int n)
{
	int i;
	double sum = 0;
	for(i=0; i<n; ++i)
		sum += vars[i][0]*vars[i][0] - 10*cos(2*pi*vars[i][0]) + 10;
	return sum;
}

/* ����2��ָ�����ֵ */
double fun2(double **vars, int n)
{
	int i;
	double sum = 0, temp = 1;
	for(i=0; i<n; ++i)
	{
		temp *= cos(vars[i][0]/sqrt(i+1));
		sum += vars[i][0]*vars[i][0];
	}
	sum = sum/4000 - temp + 1;
	return sum;
}

// �����Լ�����Ӻ�������

/* ���ݸ������������������� */
double ** cre_mat(int x, int y)
{
	int i, j;
	double **m = (double **)malloc(x * sizeof(double *));
	if(m == NULL) return 0;
	for(i=0; i<x; ++i)
	{
		m[i] = (double *)malloc(y * sizeof(double));
		if(m[i] == NULL) return 0;
	}

	/* ��ʼ�� */
	for(i=0; i<x; ++i)
		for(j=0; j<y; ++j)
			m[i][j] = 0.0;

	return m;
}

/* �ͷž������õ��ڴ� */
void free_mat(double **m, int x)
{
	int i;
	for(i=0; i<x; ++i)
		free(m[i]);
	free(m);
}

/* �������� */
void mat_tra(double **m1, double **m2, int x, int y)
{
	int i, j;
	for(i=0; i<x; ++i)
		for(j=0; j<y; ++j)
			m2[j][i] = m1[i][j];
}

/* ���ƾ��� */
void cpy_mat(double **m1, double **m2, int x, int y)
{
	int i, j;
	for(i=0; i<x; ++i)
		for(j=0; j<y; ++j)
			m2[i][j] = m1[i][j];
}

/* ������� */
void mat_add(double **m1, double **m2, double **m3, int x, int y)
{
	int i, j;
	for(i=0; i<x; ++i)
		for(j=0; j<y; ++j)
			m3[i][j] = m1[i][j] + m2[i][j];
}

/* ������� */
void mat_sub(double **m1, double **m2, double **m3, int x, int y)
{
	int i, j;
	for(i=0; i<x; ++i)
		for(j=0; j<y; ++j)
			m3[i][j] = m1[i][j] - m2[i][j];
}

/* ������� */
void mat_mul(double **m1, double **m2, double **m3, int x, int y, int z)
{
	int i, j, k;

	/* ��ʼ��������� */
	for(i=0; i<x; ++i)
		for(j=0; j<z; ++j)
			m3[i][j] = 0.0;

	for(i=0; i<x; ++i)
		for(j=0; j<z; ++j)
			for(k=0; k<y; ++k)
				m3[i][j] += m1[i][k]*m2[k][j];
}

/* �������� */
void mat_sca_mul(double a, double **m, int x, int y)
{
	int i, j;
	for(i=0; i<x; ++i)
		for(j=0; j<y; ++j)
				m[i][j] = a * m[i][j];
}

/* �ü��޷�����ָ�����ƫ��/�ݶ� */
void differ(double (*f)(double **vars, int n), double **vars, double **grads, int n, double prec)
{
	int i, j;
	double delta_x = 3, delta_y;	// ��x�ͦ�y����x��ֵ��Ϊ3
	double y1, y2;
	double diff1, diff2;
	double **vars_temp = cre_mat(n, 1);

	/* �ֱ���xÿ��������ƫ�� */
	for(i=0; i<n; ++i)
	{
		y1 = f(vars, n);
		for(j=0; j<n; ++j)
		{
			vars_temp[j][0] = vars[j][0];
			if(i == j) vars_temp[j][0] += delta_x;	// ֻ��Ե�ǰ��x�������Ϧ�x
		}
		y2 = f(vars_temp, n);
		delta_y = y2 - y1;
		diff1 = delta_y / delta_x;	// ��һ����ֵ

		while(1)
		{
			delta_x = 0.5 * delta_x;	// ��С��x
			y1 = f(vars, n);
			for(j=0; j<n; ++j)
			{
				vars_temp[j][0] = vars[j][0];
				if(i == j)
					vars_temp[j][0] += delta_x;
			}
			y2 = f(vars_temp, n);
			delta_y = y2 - y1;
			diff2 = delta_y / delta_x;	// ����һ����ֵ

			/* 
			* ������α�ֵ֮��ľ���ֵС���趨�ľ���ֵ��
			* ��ڶ�����ֵ��Ϊ�÷�����ƫ����
			* ���߲�����С��x��ֱ������������ֵ����
			*/
			if(fabs(diff2 - diff1) <= prec)
				break;
			else
				diff1 = diff2;
		}
		grads[i][0] = diff2;	// ���������ƫ��ֵ
	}

	/* �ͷŶ��ڴ� */
	free_mat(vars_temp, n);
	vars_temp = NULL;
}

/* �ɹ�ʧ�ܷ�������һά���� */
void suc_fail(double (*f)(double **vars, int n), double **vars, double **d, int n, double prec, double h)
{
	int i;
	double alpha = 0;		// ��������ֵ��Ϊ0������0��
	double f1, f2;
	double **vars_temp1 = cre_mat(n, 1);
	double **vars_temp2 = cre_mat(n, 1);

	/* ���ݰ������Ͳ������Ӧ��xֵ */
	for(i=0; i<n; ++i)
	{
		vars_temp1[i][0] = vars[i][0] - alpha * d[i][0];
		vars_temp2[i][0] = vars[i][0] - (alpha+h) * d[i][0];
	}
	f1 = f(vars_temp1, n);

	/* ����ѭ�� */
	while(1)
	{
		f2 = f(vars_temp2, n);

		/* f2С��f1���Ӵ󲽳�����һ��ǰ�� */
		if(f2 < f1)		
		{
			alpha += h;		// ���°�����
			for(i=0; i<n; ++i)	// ���¶�Ӧ��xֵ
				vars_temp1[i][0] = vars[i][0] - alpha * d[i][0];
			f1 = f2;
			h = 2*h;	// ��������һ��
			for(i=0; i<n; ++i)	// ���¶�Ӧ��xֵ
				vars_temp2[i][0] = vars[i][0] - (alpha+h) * d[i][0];
		}

		/* ���㾫��Ҫ�� */
		else if(fabs(h) <= prec)	
		{
			/* ���氢������Ӧ��xֵ */
			for(i=0; i<n; ++i)
				vars[i][0] = vars_temp1[i][0];

			break;	// �㷨�������˳�
		}

		/* f2����f1����С��������һ�κ��� */
		else	
		{
			h = -(h/4);		// ���²���
			for(i=0; i<n; ++i)	// ���¶�Ӧ��xֵ
				vars_temp2[i][0] = vars[i][0] - (alpha+h) * d[i][0];
		}
	}	

	/* �ͷ������ݴ�Ķ��ڴ� */
	free_mat(vars_temp1, n);
	vars_temp1 = NULL;
	free_mat(vars_temp2, n);
	vars_temp2 = NULL;
}

/* �����½�����Speedest Descent Method��*/
void SD(int fun, int n, double prec, double h)
{
	int i, iter = 0;
	double norm;	// �ݴ��ݶȵķ���

	double **vars = cre_mat(n, 1);	// �ݴ����x�ľ���
	double **grads = cre_mat(n, 1);	// �ݴ��ݶȵľ���

	/* ��ʾ�������x�ĳ�ֵ */
	printf("Please input the initial value: ");
	fflush(stdin);	// ��ձ�׼IO���棬��ֹ������������Ķ�ȡ
	for(i=0; i<n; ++i)
		scanf("%lf", &vars[i][0]);

	printf("\nIterating...\n");	// ������
	/* ����ѭ�� */
	while(1)
	{
		++iter;
		printf("iter=%d\n", iter);	// ���iter��ָʾ��������

		switch(fun)	// ��ָ��������ƫ��
		{
		case 1:	differ(fun1, vars, grads, n, prec);break;
		case 2:	differ(fun2, vars, grads, n, prec);break;
		default: break;
		}

		/* ���ݶȵĶ����� */
		norm = 0;
		for(i=0; i<n; ++i)
			norm += grads[i][0] * grads[i][0];
		norm = sqrt(norm);

		/* �ݶȷ������㾫�� */
		if(norm <= prec)
		{
			/* ������ */
			printf("\nThe answer: \n");
			for(i=0; i<n; ++i)
				printf("x%d = %g \n", i+1, vars[i][0]);
			switch(fun)
			{
			case 1:	printf("min(f1) = %g \n", fun1(vars, n));break;
			case 2:	printf("min(f2) = %g \n", fun2(vars, n));break;
			default: break;
			}

			/* �Ƿ�������ݶȵ��ڴ� */
			free_mat(vars, n);
			vars = NULL;
			free_mat(grads, n);
			grads = NULL;

			break;	// �㷨�������˳�
		}
		/* �ݶȷ��������㾫�ȣ�����һά���� */
		else
		{
			switch(fun)	// ��ָ����������һά����
			{
			case 1:	suc_fail(fun1, vars, grads, n, prec, h);break;
			case 2:	suc_fail(fun2, vars, grads, n, prec, h);break;
			default: break;
			}
		}
	}
	
}

/* DFP��ţ�ٷ� */
void DFP(int fun, int n, double prec, double h)
{
	int i, j, iter = 0, flag = 0;
	double norm, a, b;

	/* ����x���ݶ�g����������d�ȵľ��� */
	double **vars=cre_mat(n, 1), **grads=cre_mat(n, 1), **H=cre_mat(n, n), **d=cre_mat(n, 1);
	double **vars_temp=cre_mat(n, 1), **grads_temp=cre_mat(n, 1);

	/* ��H���ݴ���� */
	double **a_temp=cre_mat(1, 1), **dgt_H=cre_mat(1, n), **b_temp=cre_mat(1, 1);
	double **delta_x=cre_mat(n, 1), **delta_g=cre_mat(n, 1);
	double **delta_x_t=cre_mat(1, n), **delta_g_t=cre_mat(1, n);
	double **H_dg=cre_mat(n, 1), **H_dg_dgt=cre_mat(n, n);
	double **H_temp1=cre_mat(n, n), **H_temp2=cre_mat(n, n);
	
	/* ��ʾ�������x�ĳ�ֵ */
	printf("Please input the initial value: ");
	fflush(stdin);	// ��ձ�׼IO���棬��ֹ������������Ķ�ȡ
	for(i=0; i<n; ++i)
		scanf("%lf", &vars[i][0]);
	
	printf("\nIterating...\n");	// ������
	/* ����ѭ�� */
	while(1)
	{
		++iter;	
		printf("iter=%d\n", iter);	// ���iter��ָʾ��������
		++flag;	// ��ʶ�Ƿ�Ҫ��H��iter��2���Լ��Ƿ�Ҫ���µ���

		if(1 == flag)
		{
			/* ��ʼ��HΪ��λ���� */
			for(i=0; i<n; ++i)
				for(j=0; j<n; ++j)
					if(i == j)
						H[i][j] = 1;
		}

		cpy_mat(grads, grads_temp, n, 1);	// �����ݶȣ�������g
		switch(fun)	// ��ָ��������ƫ��
		{
		case 1:	differ(fun1, vars, grads, n, prec);break;
		case 2:	differ(fun2, vars, grads, n, prec);break;
		default: break;
		}

		/* ���ݶȵĶ����� */
		norm = 0;
		for(i=0; i<n; ++i)
			norm += grads[i][0] * grads[i][0];
		norm = sqrt(norm);

		/* �ݶȷ������㾫��Ҫ�� */
		if(norm <= prec)
		{
			/* ������ */
			printf("\nThe answer: \n");
			for(i=0; i<n; ++i)
				printf("x%d = %g \n", i+1, vars[i][0]);
			switch(fun)
			{
			case 1:	printf("min(f1) = %g \n", fun1(vars, n));break;
			case 2:	printf("min(f2) = %g \n", fun2(vars, n));break;
			default: break;
			}

			// �ͷ��ڴ棨���ڿ��ܶ�����У������ͷţ�����й¶���³����쳣�˳���
			free_mat(vars, n);
			vars = NULL;
			free_mat(grads, n);
			grads = NULL;
			free_mat(H, n);
			H = NULL;
			free_mat(d, n);
			d = NULL;

			free_mat(a_temp, 1);
			a_temp = NULL;
			free_mat(dgt_H, 1);
			dgt_H = NULL;
			free_mat(b_temp, 1);
			b_temp = NULL;
			
			free_mat(vars_temp, n);
			vars_temp = NULL;
			free_mat(grads_temp, n);
			grads_temp = NULL;
			free_mat(delta_x, n);
			delta_x = NULL;
			free_mat(delta_g, n);
			delta_g = NULL;
			free_mat(delta_x_t, 1);
			delta_x_t = NULL;
			free_mat(delta_g_t, 1);
			delta_g_t = NULL;
				
			free_mat(H_dg, n);
			H_dg = NULL;
			free_mat(H_dg_dgt, n);
			H_dg_dgt = NULL;
			free_mat(H_temp1, n);
			H_temp1 = NULL;
			free_mat(H_temp2, n);
			H_temp2 = NULL;
			
			break;	// �㷨�������˳�
		}

		/* �ݶȷ��������㾫��Ҫ�󣬼������� */
		else
		{
			/* ��H */
			if(flag > 1)
			{
				if(n == flag)	//	���������ﵽn����ʼ��һ�ֵ���
					flag = 0;
				else
				{
					/* ��x�ͦ�g */
					mat_sub(vars, vars_temp, delta_x, n, 1);
					mat_sub(grads, grads_temp, delta_g, n, 1);
					
					/* ��x�ͦ�g��ת�� */
					mat_tra(delta_x, delta_x_t, n, 1);
					mat_tra(delta_g, delta_g_t, n, 1);
					
					/* �ֲ���H */
					mat_mul(delta_g_t, H, dgt_H, 1, n, n);
					mat_mul(dgt_H, delta_g, a_temp, 1, n, 1);
					a = a_temp[0][0];
					a = 1/a;
					
					mat_mul(delta_x_t, delta_g, b_temp, 1, n, 1);
					b = b_temp[0][0];
					b = 1/b;
					
					mat_mul(H, delta_g, H_dg, n, n, 1);
					mat_mul(H_dg, delta_g_t, H_dg_dgt, n, 1, n);
					mat_mul(H_dg_dgt, H, H_temp1, n, n, n);
					mat_sca_mul(a, H_temp1, n, n);
					
					mat_mul(delta_x, delta_x_t, H_temp2, n, 1, n);
					mat_sca_mul(b, H_temp2, n, n);
					
					/* �����յ���H */
					mat_sub(H, H_temp1, H, n, n);
					mat_add(H, H_temp2, H, n, n);
				}
			}

			if(flag > 0)	// �������һ�ֵ���(flag==0)������������Ĵ���
			{
				mat_mul(H, grads, d, n, n, 1);	// ��������������δ�Ӹ��ţ��ڳɹ�ʧ���㷨��ӣ�
				cpy_mat(vars, vars_temp, n, 1);	// ���ݱ�����������x
				
				switch(fun)	// ��ָ����������һά����
				{
				case 1:	suc_fail(fun1, vars, d, n, prec, h);break;
				case 2:	suc_fail(fun2, vars, d, n, prec, h);break;
				default: break;
				}
			}
		}
	}
}

/* ������ */
int main()
{
	int n, met, fun;	// met-method-������fun-function-����
	double prec, h;	// �㷨����prec��һά�����Ĳ���h

	while(1)
	{
		do
		{	/* ѡ�񷽷��������½�����DFP */
			printf("\nPlease select the methord(1 or 2 , 0 to quit): \n");
			printf(" 0: quit \n 1: SD(steepest descent) \n 2: DFP \n");
			printf("Your selected methord: ");
			fflush(stdin);	// ��ձ�׼IO���棬��ֹ������������Ķ�ȡ
			scanf("%d", &met);
		}while(met<0 || met >2); // ѡ���������ѡ��
		if(met == 0) break;	// �û�ѡ���˳�

		do
		{	/* ѡ���� */
			printf("\nPlease select the function(1 or 2): \n");
			printf(" 1: f1=��[x^2-10*cos(2*��*x)+10] \n");
			printf(" 2: f2=(��x^2)/4000-��cos(x/i)+1] \n");
			printf("Your selected function: ");
			fflush(stdin);	// ��ձ�׼IO���棬��ֹ������������Ķ�ȡ
			scanf("%d", &fun);
		}while(fun<1 || fun>2);	// ѡ���������ѡ��
		
		do
		{
			/* �����㷨���� */
			printf("\nPlease input the precision(prec>0): ");
			fflush(stdin);	// ��ձ�׼IO���棬��ֹ������������Ķ�ȡ
			scanf("%lf", &prec);
		}while(prec<=0);	// ά��������������

		do
		{
			/* �����㷨���� */
			printf("Please input the step size(h>0): ");
			fflush(stdin);	// ��ձ�׼IO���棬��ֹ������������Ķ�ȡ
			scanf("%lf", &h);
		}while(prec<=0);	// ά��������������

		do
		{
			/* �������ά�� */
			printf("\nPlease input the dimension(n>=1): ");
			fflush(stdin);	// ��ձ�׼IO���棬��ֹ������������Ķ�ȡ
			scanf("%d", &n);
		}while(n<1);	// ά��������������

		switch(met)	// ����ָ�������ĺ���
		{
		case 1: SD(fun, n, prec, h);break;
		case 2: DFP(fun, n, prec, h);break;
		default: break;
		}
	}
}
