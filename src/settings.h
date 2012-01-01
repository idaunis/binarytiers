/* Base URL. Set to NULL for autodetect */
char * BASE_URL			= (char *) "http://mysite.domain";

/* Default Temporary directory */
char * TEMP_PATH		= (char *) "temp"; // You can set this later on the Settings option

/* MYSQL Settings */
char * DB_HOST			= (char *) "127.0.0.1";
char * DB_USER			= (char *) "my_user";
char * DB_PASSW			= (char *) "my_password";
char * DB_NAME			= (char *) "my_database";
int	   DB_TYPE			= 1; // 1:Redis, 2:Mysql
int	   DB_INDEX			= 0;

/* Sendmail Settings */
char * SENDMAIL_HOST	= (char *) "127.0.0.1";
char * SENDMAIL_FROM	= (char *) "Webmaster <webmaster@mysite.domain>";

/* Set maximum uploading size (in bytes) */
int max_content_length  = 200000000;

/* Cookie life time in seconds */
int	cookie_lifetime		= 2000000;

/* Use clean URLs */
int	clean_url			= 1;

/* Enable to use Fast CGI */
//#define FASTCGI
