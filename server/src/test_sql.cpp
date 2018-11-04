#include <stdio.h>
#include "libpq-fe.h"

// 输出打印PGresult结果
void PQresultPrint(PGresult *res)
{
	int nFields = PQnfields(res);
	int nTuples = PQntuples(res);
	int i, j;
	for (i=0; i<nTuples; i++)
	{
		for (j=0; j<nFields; j++)
		{
			printf("%s ", PQgetvalue(res, i, j));
		}
		printf("/n");
	}
}


// 编译命令:
PGresult *ExecuteQuery(char *host, int port, char *dbname, char *query)
{

	PGconn *conn;
	PGresult *res;
	
	char str[128];
	sprintf(str, "host=%s port=%d dbname=%s user=%s password=%s", host, port, dbname, "lcy", "lcy");
	
	// 建立连接
	conn = PQconnectdb(str);
	if(PQstatus(conn) == CONNECTION_BAD)
	{
		fprintf(stderr,"数据库连接失败！ host: %s/n", host);
		fprintf(stderr,"%s",PQerrorMessage(conn));
	}
	
	// 执行SQL
	res = PQexec(conn, query);
	if (PQresultStatus(res) == PGRES_FATAL_ERROR)
	{
		fprintf(stderr, "%s", PQerrorMessage(conn));
	}
	
	// 命令语句：CREATE, UPDATE, DELETE等
	if (PQresultStatus(res) == PGRES_COMMAND_OK)
	{
		printf("%s/n", PQcmdTuples(res));
	}
	
	// SELECT SQL
	if (PQresultStatus(res) == PGRES_TUPLES_OK)
	{

	}
	
	PQfinish(conn);
	return res;
}

int main()
{
	char *sql = "SELECT * FROM student;";
	PGresult *res = ExecuteQuery("127.0.0.1", 5432, "test", sql);
	PQresultPrint(res);
	PQclear(res);
	return 0;
}
