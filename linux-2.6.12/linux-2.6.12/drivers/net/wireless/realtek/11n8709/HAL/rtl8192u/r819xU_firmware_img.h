/******************************************************************************
 * Copyright(c) 2008 - 2010 Realtek Corporation. All rights reserved.
 * Linux device driver for RTL8192U 
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
******************************************************************************/
#ifndef IMG_H
#define IMG_H

#define BOOT_ARR_LEN 344
#define MAIN_ARR_LEN 46888
#define DATA_ARR_LEN 800
#define MACPHY_Array_PGLength 30
#define PHY_REG_1T2RArrayLength 296
#define AGCTAB_ArrayLength 384
#define MACPHY_ArrayLength 18

#define RadioA_ArrayLength 246
#define RadioB_ArrayLength 78
#define RadioC_ArrayLength 1
#define RadioD_ArrayLength 1
#define PHY_REGArrayLength 1


extern u8 rtl8190_fwboot_array[BOOT_ARR_LEN];
extern u8 rtl8190_fwmain_array[MAIN_ARR_LEN];
extern u8 rtl8190_fwdata_array[DATA_ARR_LEN];

extern u32 Rtl8192UsbPHY_REGArray[];
extern u32 Rtl8192UsbPHY_REG_1T2RArray[];
extern u32 Rtl8192UsbRadioA_Array[];
extern u32 Rtl8192UsbRadioB_Array[];
extern u32 Rtl8192UsbRadioC_Array[];
extern u32 Rtl8192UsbRadioD_Array[];
extern u32 Rtl8192UsbMACPHY_Array[];
extern u32 Rtl8192UsbMACPHY_Array_PG[];
extern u32 Rtl8192UsbAGCTAB_Array[];



#endif
