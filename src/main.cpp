/* BinaryTiers Settings */
#include "../lib/core.h"
#include "settings.h"

/* Custom Themes */
#include "../themes/admin/admin.h"
#include "../themes/site/site.h"

/* Custom Models */
#include "example.h"

int main(int argc, char *argv[])
{
#ifdef FASTCGI
	while (FCGI_Accept() >= 0)   {
#endif
		bt_begin( argc, argv );

		_MODEL( system );
		_MODEL( node );
		_MODEL( user );
		_MODEL( profile );
		_MODEL( ckeditor );
		_MODEL( taxonomy );
		_MODEL( upload );
		_MODEL( locale );
		_MODEL( translation );
		_MODEL( aggregator );
		_MODEL( path );
		_MODEL( comment );
		_MODEL( menu );

		_MODEL( example );
		
		_THEME( admin );
		_THEME( site );

		bt_end();
#ifdef FASTCGI
	}
#endif
}
