/***************************************************************************
 *   hddKeyboardLed 0.1                                                    *
 *   Copyright (C) 2005 by Matthias Reif                                   *
 *   matthias.reif@informatik.tu-chemnitz.de                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
using namespace std;

#define SCR_LED_ON 1
#define SCR_LED_OFF 254

int setScrollLed( int fd, int on )
{
	char ledStatus;
	// save old led state
	if( ioctl( fd, KDGETLED, &ledStatus ) < 0 )
	{
		perror("ioctl() KDGETLED");
		return -1;
	}
	if( on )
		// switch scroll led on
		ledStatus = ledStatus | SCR_LED_ON;
	else
		// switch scroll led off
		ledStatus = ledStatus & SCR_LED_OFF;

	if( ioctl( fd, KDSETLED, ledStatus ) < 0 )
	{
		perror("ioctl() KDSETLED");
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[]) 
{
	// check params
	if( ( argc < 2 ) || ( strcmp( argv[1], "--help" ) ) == 0 )
	{
		cout << "Usage:    " << argv[ 0 ] << " [device] .." << endl;
		cout << "Example:  " << argv[ 0 ] << " hda hdb hdd" << endl;
		return 0;
	}

	ifstream hdFile;
	int numDevices = argc - 1;
	char* path[ numDevices ];
	int device;

	// build and check path for each device
	for( device = 0; device < numDevices; device++ )
	{
		path[ device ] = new char[ strlen( argv[ 1 + device ] ) + strlen( "/sys/block//stat\0" ) + 1 ];
		sprintf( path[ device ], "%s%s%s", "/sys/block/", argv[ 1 + device ], "/stat\0" );

		hdFile.open( path[ device ] );
		if( ! hdFile.good() )
		{
			cout << "unable to read device " << argv[ 1 + device ] << endl;
			cout << "ensure that sysfs is mounted" << endl;
			return 1;
		}
		hdFile.close();
	}

	int ledFd;
	ledFd = open("/dev/console", O_RDONLY);
	if( ledFd == -1 )
	{
		perror("opening /dev/console");
		return 1;
	}

	int x[ 11 ];
	int i;
	while( 1 )
	{
		// read stats of each device
		for( device=0; device<numDevices; device++ )
		{
			hdFile.open( path[ device ] );
			for( i=0; i<11; i++ )
			{
				hdFile >> x[i];
			}
			hdFile.close();
			// break if any device has any activity
			if( x[ 8 ] > 0 )
				break;
		}
		if( setScrollLed( ledFd, x[8] ) < 0 )
			return 1;
		usleep( 50000 );
	}

	close( ledFd );
	for( device=0; device<numDevices; device++ )
	{
		delete path[ device ];
	}
	return 0;
}
