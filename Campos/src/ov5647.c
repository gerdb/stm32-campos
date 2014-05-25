/**
 *  Project     Campos
 *  @file		ov5647.c
 *  @author		Gerd Bartelt - www.sebulli.com
 *  @brief		Omivision OV5647 driver
 *
 *  @copyright	GPL3
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ov5647.h"

/* Local variables  ----------------------------------------------------------*/
static struct regval_list ov5647_regs[] = {
//
		{ 0x0100, 0x00 }, // Software sleep command
		{ 0x0103, 0x01 }, // Software reset
		{ REG_DLY, 005 }, // 5ms delay
		{ 0x3013, 0x08 }, // Bit[7:4]: Debug control Changing these registers is not 		recommended
						  // Bit[3]: bp_regulator
						  // 0: Enable internal regulator
						  // 1: Disable internal regulator
						  // Bit[2:0]: Debug control Changing these registers is not 		recommended
		{ 0x3018, 0x00 }, // Bit[6]: mipi_en
						  // 0: DVP enable
						  // 1: MIPI enable
		{ 0x5000, 0x03 }, // Bit[7] lenc_en
						  // 0: Disable
						  // 1: Enable
						  // Bit[2]: bc_en
						  // 0: Disable
						  // 1: Enable
						  // Bit[1]: wc_en
						  // 0: Disable
						  // 1:	Enable
		{ 0x5001, 0x00 }, // Bit [0]: awb_en
						  // 0: Disable
						  // 1:	Enable
		{ 0x5a00, 0x08 }, // Bit [2]: dig_comp_bypass
						  // Bit [1]: man_opt
						  // Bit [0]: man_en
		{ 0x3c01, 0x80 }, // Bit [7]: band_man_en - Band detection manual mode
						  // 0: Manual mode disable
						  // 1: Manual mode enable
						  // Bit[6:0]: 50/60 Hz detection control
		{ 0x3b07, 0x09 }, // Bit[4]: frex_sa1
						  // Bit[3]: fx1_fm_en
						  // Bit[2]: frex_inv
						  // Bit[1:0]: FREX strobe
						  // 00: frex_strobe mode0
						  // 01: frex_strobe mode1
						  // 1x: Rolling strobe
		{ 0x3a08, 0x00 }, // Bit[1:0]: b50_step[9:8]
		{ 0x3a0a, 0x00 }, // Bit[7:0]: b50_step[7:0]
		{ 0x3a0f, 0x58 }, // Bit[7:0]: WPT  Stable range high limit (enter)
		{ 0x3a10, 0x50 }, // Bit[7:0]: BPT  Stable range low limit (enter)
		{ 0x3a1b, 0x58 }, // Bit[7:0]: wpt2 Stable range high limit (go out)
		{ 0x3a1e, 0x50 }, // Bit[7:0]: bpt2 Stable range low limit (go out)
		{ 0x3a11, 0x60 }, // Bit[7:0]: vpt_high
		{ 0x3a1f, 0x28 }, // Bit[7:0]: vpt_low  Step manual mode, fast zone low limit
		{ 0x0100, 0x01 }, // Software sleep/wakeup command
		{ 0x4000, 0x89 }, // Bit[7]: blc_median_filter_enable
						  // Bit[3]: adc_11bit_mode
						  // Bit[2]: apply2blackline
						  // Bit[1]: blackline_averageframe
						  // Bit[0]: BLC enable
		{ 0x4001, 0x02 }, // Bit[5:0]: start_line
		{ 0x4002, 0xc5 }, // Bit[7]: format_change_en
						  // format_change_i from fmt will be effect when it is enable
						  // Bit[6]: blc_auto_en
						  // Bit[5:0]: reset_frame_num
		{ 0x4004, 0x06 }, // Bit[7:0]: blc_line_num
		{ 0x4005, 0x1a }, // Bit[5]: one_line_mode
						  // Bit[4]: remove_none_imagedata
						  // Bit[3]: blc_man_1_en
						  // Bit[2]: blackline_bggr_man_en
						  // 0: bgbg/grgr is decided by rblue/hswap
						  // 1: bgbg/grgr fix
						  // Bit[1]: bgbg/grgr is decided by rblue/hswap
						  // blc_always_up_en
						  // 0: Normal freeze
						  // 1: BLC a
		{ 0x3503, 0x13 }, // Bit[5:4]: Gain latch timing delay
						  // x0: Gain has no latch delay
						  // 01: Gain delay of 1 frame
						  // 11: Gain delay of 2 frames
						  // Bit[2]: VTS manual
						  // 0: Auto enable
						  // 1: Manual enable
						  // Bit[1]: AGC manual
						  // 0: Auto enable
						  // 1: Manual enable
						  // Bit[0]: AEC manual
						  // 0: Auto enable
						  // 1: Manual enable
		{ 0x3500, 0x01 }, // Bit[3:0]: Exposure[19:16]
		{ 0x3501, 0x00 }, // Bit[7:0]: Exposure[15:8]
		{ 0x3502, 0x08 }, // Bit[7:0]: Exposure[7:0]
		{ 0x350a, 0x02 }, // Bit[1:0]: Gain[9:8] AGC real gain output high byte
		{ 0x350b, 0x7f }, // Bit[7:0]: Gain[7:0] AGC real gain output low byte
		{ 0x350c, 0x00 }, // vts diff manual vts set to 0
		{ 0x350d, 0x00 }, // vts diff manual vts set to 0
		{ 0x3011, 0x22 }, // bit[6:5] drive strength
		{ 0x3035, 0x21 }, // Bit[7:4]: system_clk_div
						  // Will slow down all clocks
						  // Bit[3:0]: scale_divider_mipi
						  // MIPI PCLK/SERCLK can be slowed down when image is  scaled down
		{ 0x3036, 0x80 }, // Bit[7:0]: PLL_multiplier (4~252) can be  any integer during 4~127
						  // and only even integer during 128~252
		{ 0x303c, 0x12 }, // Bit[6:4]: plls_cp
						  // Bit[3:0]: plls_sys_div
		{ 0x3820, 0x41 }, // Bit[2]: r_vflip_isp
						  // Bit[1]: r_vflip_snr
						  // Bit[0]: r_vbin
		{ 0x3821, 0x07 }, // Bit[2]: r_vmirror_isp
						  // Bit[1]: r_vmirror_snr
						  // Bit[0]: r_hbin
		{ 0x380c, 0x0a }, // Bit[4:0]: Total horizontal size[12:8] (2710dec)
		{ 0x380d, 0x96 }, // Bit[7:0]: Total horizontal size[7:0]
		{ 0x380e, 0x07 }, // Bit[1:0]: Total vertical size[9:8] (1968dec)
		{ 0x380f, 0xb0 }, // Bit[7:0]: Total vertical size[7:0]
		{ 0x3814, 0x11 }, // Bit[7:4]: h_odd_inc Horizontal subsample odd increase number
						  // Bit[3:0]: h_even_inc Horizontal subsample even increase number
		{ 0x3815, 0x11 }, // 0x3815 Bit[7:4]: v_odd_inc Vertical subsample odd increase number
						  // Bit[3:0]: v_even_inc Vertical subsample even increase number
		{ 0x3808, 0x0a }, // Bit[3:0]: DVP output horizontal width[11:8] (2592=0x0A20, 160=0xA0, 320=0x140)
		{ 0x3809, 0x20 }, // Bit[7:0]: DVP output horizontal width[7:0]
		{ 0x380a, 0x07 }, // Bit[3:0]: DVP output vertical height[11:8] (1944=0x0798, 120=0x78)
		{ 0x380b, 0x98 }, // Bit[7:0]: DVP output vertical height[7:0]
		{ 0x3800, 0x00 }, // Bit[3:0]: x_addr_start[11:8] (12dec)
		{ 0x3801, 0x0c }, // Bit[7:0]: x_addr_start[7:0]
		{ 0x3802, 0x00 }, // Bit[3:0]: y_addr_start[11:8] (4dec)
		{ 0x3803, 0x04 }, // Bit[7:0]: y_addr_start[7:0]
		{ 0x3804, 0x0a }, // Bit[3:0]: x_addr_end[11:8] (2611dec)
		{ 0x3805, 0x33 }, // Bit[7:0]: x_addr_end[7:0]
		{ 0x3806, 0x07 }, // Bit[3:0]: y_addr_end[11:8] (1955dec)
		{ 0x3807, 0xa3 }, // Bit[7:0]: y_addr_end[7:0]
		{ 0x3000, 0x0f }, // Bit[7:4]: io_y_oen[11:8]
		{ 0x3001, 0xff }, // Bit[7:0]: io_y_oen[7:0]
		{ 0x3002, 0xe4 }, // Bit[7]: io_vsync_oen
		  	  	  	  	  // Bit[6]: io_href_oen
	  	  	  	  	  	  // Bit[5]: io_pclk_oen
	  	  	  	  	  	  // Bit[4]: io_frex_oen
	  	  	  	  	  	  // Bit[3]: io_strobe_oen
	  	  	  	  	  	  // Bit[2]: io_sda_oen
	  	  	  	  	  	  // Bit[1]: io_gpio1_oen
	  	  	  	  	  	  // Bit[0]: io_gpio0_oen
		};

/*
 * ;
 */
/**
 * @brief  Write a list of register settings.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  regs: list with register and data pairs
 * @retval None
 */
static void sensor_write_array(uint16_t DeviceAddr, struct regval_list *regs,
		int array_size) {
	int i = 0;

	while (i < array_size) {
		if (regs->addr == REG_DLY) {
			CAMERA_Delay(regs->data);
		} else {
			CAMERA_IO_Write(DeviceAddr, regs->addr, regs->data);
		}
		i++;
		regs++;
	}
}

/**
 * @brief  Initializes the OV5647 CAMERA component.
 * @param  DeviceAddr: Device address on communication Bus.
 * @param  resolution: the Camera resolution
 * @retval None
 */
void ov5647_Init(uint16_t DeviceAddr) {

	// Initialize I2C
	CAMERA_IO_Init();

	sensor_write_array(DeviceAddr, ov5647_regs, ARRAY_SIZE(ov5647_regs));
}

/**
 * @brief  Read the OV5647 Camera ID
 * @param  DeviceAddr: Device address on communication Bus.
 * @retval the OV5647 ID
 */
uint16_t ov5647_ReadID(uint16_t DeviceAddr) {
	uint16_t result = 0;

	/* Get the camera ID */
	result = CAMERA_IO_Read(DeviceAddr, OV5647_SC_CMMN_CHIP_ID_H);
	result <<= 8;
	result |= CAMERA_IO_Read(DeviceAddr, OV5647_SC_CMMN_CHIP_ID_L);

	return result;
}

