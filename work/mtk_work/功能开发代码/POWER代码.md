# Power电源控制

- lk代码操作片段

  ```code
  //读原型
  pmic_read_interface(unsigned int RegNum, unsigned int *val, unsigned int MASK, unsigned int SHIFT)
  
  //读取的是两个bit，所以 mask 是 11 即 0x3，因为读取的是 bit4~5,所以 shift 是4;
  pmic_read_interface(0x330,&val,0x3,4);


  //写原型
  pmic_config_interface(unsigned int RegNum, unsigned int val, unsigned intMASK, unsigned int SHIFT)

  //直接设置0x330的bit4~5为 10，即0x2：
  pmic_config_interface(0x330,0x2,0x3,4);

  ```

- 内核代码操作片段

  ```code
  if (pmic_get_register_value(PMIC_DA_VCORE_VOSEL) != vcore_vosel)
    pr_err("vmd1_pmic_setting_on vcore vosel = 0x%x, da_vosel = 0x%x",
          pmic_get_register_value(PMIC_RG_BUCK_VCORE_VOSEL),
          pmic_get_register_value(PMIC_DA_VCORE_VOSEL));

  /* 1.Call PMIC driver API configure VMODEM voltage */
  pmic_set_register_value(PMIC_RG_BUCK_VMODEM_VOSEL, vmodem_vosel);
  if (pmic_get_register_value(PMIC_DA_VMODEM_VOSEL) != vmodem_vosel)
    pr_err("vmd1_pmic_setting_on vmodem vosel = 0x%x, da_vosel = 0x%x",
          pmic_get_register_value(PMIC_RG_BUCK_VMODEM_VOSEL),
          pmic_get_register_value(PMIC_DA_VMODEM_VOSEL));

  ```
