# audio

## 原理图理解

### 耳机座
- 检测耳机插入检测
    HP_DET_H    GPIO4_D4_D
    PHONE_DET   codec 的irq
- 耳机上的按键hook
    HP_HOOK     ADC_IN2
- 耳机座左右声道
    HPOR和HPOL连接到 codec上的 HPO_L 和 HPO_R
- 耳机座上的MIC
    MIC_IN1P    连接到codec的   IN3P
    HP_HOOK 和 MIC_IN1P在一根线上
- 耳机座micbias
    MICBIAS1    连接到codec的   MICBIAS1

### 板子上的mic
- MIC_IN2P 和 MIC_IN2N 接在 codec的 IN2P 和 IN2N
- micbias
    MICBIAS1    连接到codec的   MICBIAS1 
    和耳机座的mic公用了bias1

### SPDIF
- SPDIF_TX 连接 GPIO4_C5/SPDIF_TX_D
- VCC5V0_SYS

### SPEAKER PA芯片 PT1505EMSH
- HPOL 和 HPOR 使用的就是耳机的左右声道
- PA使能脚 SPK_CTL_H 接在  GPIO0_B3_D

### codec ALC5651 芯片
- VCCA3V0_CODEC （RK808 VLDO4）、VCCA1V8_CODEC（RK808 VLDO3 ）
- i2c通讯1.8v  I2C_SCL_AUDIO、 I2C_SDA_AUDIO   （VCCA1V8_CODEC上拉 ）
    GPIO4_A2/I2C1_SCL
    GPIO4_A1/I2C1_SDA
- i2s
     MCLK       - GPIO4_A0/I2S_CLK_D
     BCLK1      - GPIO3_D0/I2S0_SCLK_D
     LRCK1      - GPIO3_D2/I2S0_LRCK_TX_D / GPIO3_D1/I2S0_LRCK_RX_D
     ADCDAT1    - GPIO3_D3/I2S0_SDI0_D
     DACDAT1    - GPIO3_D7/I2S0_SDO0_D
- i2s-hdmi （HDMI转换MIPI芯片 TC358749XBG 音频输入）
     BCLK2      - 连接到HDMI转换芯片上解析HDMI里面的音频
     LRCK2      - 连接到HDMI转换芯片上解析HDMI里面的音频
     DACDAT2    - 连接到HDMI转换芯片上解析HDMI里面的音频

## 代码理解

### 参考文档
Documentation/devicetree/bindings/sound/rockchip-rt5645.txt
Documentation/devicetree/bindings/sound/rockchip-i2s.txt

### dtsi

```code
i2s0 {
	i2s0_2ch_bus: i2s0-2ch-bus {
		rockchip,pins =
			<3 RK_PD0 1 &pcfg_pull_none>,
			<3 RK_PD1 1 &pcfg_pull_none>,
			<3 RK_PD2 1 &pcfg_pull_none>,
			<3 RK_PD3 1 &pcfg_pull_none>,
			<3 RK_PD7 1 &pcfg_pull_none>,
			<4 RK_PA0 1 &pcfg_pull_none>;
	}
	i2s0_8ch_bus: i2s0-8ch-bus {
		rockchip,pins =
			<3 RK_PD0 1 &pcfg_pull_none>,
			<3 RK_PD1 1 &pcfg_pull_none>,
			<3 RK_PD2 1 &pcfg_pull_none>,
			<3 RK_PD3 1 &pcfg_pull_none>,
			<3 RK_PD4 1 &pcfg_pull_none>,
			<3 RK_PD5 1 &pcfg_pull_none>,
			<3 RK_PD6 1 &pcfg_pull_none>,
			<3 RK_PD7 1 &pcfg_pull_none>;
	}
	i2s_8ch_mclk: i2s-8ch-mclk {
		rockchip,pins = <4 RK_PA0 1 &pcfg_pull_none>;
	};
};

&i2c1 {
	status = "okay";
	i2c-scl-rising-time-ns = <300>;
	i2c-scl-falling-time-ns = <15>;

	rt5651: rt5651@1a {
		#sound-dai-cells = <0>;
		compatible = "rockchip,rt5651";
		reg = <0x1a>;
		clocks = <&cru SCLK_I2S_8CH_OUT>;
		clock-names = "mclk";
		pinctrl-names = "default";
		pinctrl-0 = <&i2s_8ch_mclk>;
		spk-con-gpio = <&gpio0 11 GPIO_ACTIVE_HIGH>;
		hp-det-gpio = <&gpio4 28 GPIO_ACTIVE_LOW>;
	};
};


//kernel\arch\arm64\boot\dts\rockchip\rk3399.dtsi
i2s0: i2s@ff880000 {
	compatible = "rockchip,rk3399-i2s", "rockchip,rk3066-i2s";
	reg = <0x0 0xff880000 0x0 0x1000>;
	rockchip,grf = <&grf>;
	interrupts = <GIC_SPI 39 IRQ_TYPE_LEVEL_HIGH 0>;
	dmas = <&dmac_bus 0>, <&dmac_bus 1>;
	dma-names = "tx", "rx";
	clock-names = "i2s_clk", "i2s_hclk";
	clocks = <&cru SCLK_I2S0_8CH>, <&cru HCLK_I2S0_8CH>;
	resets = <&cru SRST_I2S0_8CH>, <&cru SRST_H_I2S0_8CH>;
	reset-names = "reset-m", "reset-h";
	pinctrl-names = "default";
	pinctrl-0 = <&i2s0_8ch_bus>;
	power-domains = <&power RK3399_PD_SDIOAUDIO>;
	#sound-dai-cells = <0>;
	status = "disabled";
};


pinctrl: pinctrl {
	i2s0 {
		i2s0_2ch_bus: i2s0-2ch-bus {
			rockchip,pins =
				<3 RK_PD0 1 &pcfg_pull_none>,
				<3 RK_PD1 1 &pcfg_pull_none>,
				<3 RK_PD2 1 &pcfg_pull_none>,
				<3 RK_PD3 1 &pcfg_pull_none>,
				<3 RK_PD7 1 &pcfg_pull_none>,
				<4 RK_PA0 1 &pcfg_pull_none>;
		};
		i2s0_8ch_bus: i2s0-8ch-bus {
			rockchip,pins =
				<3 RK_PD0 1 &pcfg_pull_none>,
				<3 RK_PD1 1 &pcfg_pull_none>,
				<3 RK_PD2 1 &pcfg_pull_none>,
				<3 RK_PD3 1 &pcfg_pull_none>,
				<3 RK_PD4 1 &pcfg_pull_none>,
				<3 RK_PD5 1 &pcfg_pull_none>,
				<3 RK_PD6 1 &pcfg_pull_none>,
				<3 RK_PD7 1 &pcfg_pull_none>;
		};
		i2s_8ch_mclk: i2s-8ch-mclk {
			rockchip,pins = <4 RK_PA0 1 &pcfg_pull_none>;
		};
	};
};

//kernel\arch\arm64\boot\dts\rockchip\rk3399-sapphire-excavator-edp.dtsi
&rt5651_sound {
	status = "okay";
};


//kernel\arch\arm64\boot\dts\rockchip\rk3399-excavator-sapphire.dtsi
rt5651_sound: rt5651-sound {
	compatible = "simple-audio-card";
	simple-audio-card,format = "i2s";
	simple-audio-card,name = "realtek,rt5651-codec";
	simple-audio-card,mclk-fs = <256>;
	simple-audio-card,widgets =
		"Microphone", "Mic Jack",
		"Headphone", "Headphone Jack";
	simple-audio-card,routing =
		"Mic Jack", "MICBIAS1",
		"IN1P", "Mic Jack",
		"Headphone Jack", "HPOL",
		"Headphone Jack", "HPOR";
	simple-audio-card,cpu {
		sound-dai = <&i2s0>;
	};
	simple-audio-card,codec {
		sound-dai = <&rt5651>;
	};
};

```

