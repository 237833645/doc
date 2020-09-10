# st21nfc android-O kernle-3.18 mtk6739

## 通过使用st21nfc的测试程序 /vendor/bin/STFlashTool -i

  核心代码如下

  ```code
  //wangfeng add
  case ITEM_AUTO:
  memset(buf, '\0', sizeof(buf));
  stream = popen("/vendor/bin/STFlashTool -i", "r");
  LOGD(TAG "%s, 11111\n", __func__);
  if (NULL == stream)
  {
    LOGD(TAG "%s, 33333\n", __func__);
    return 0;
  }
  LOGD(TAG "%s, 22222\n", __func__);
  fread(buf, sizeof(char), sizeof(buf), stream);

  /*  
  while (fgets(buf, sizeof(char),stream) != NULL)
  {
    LOGD(TAG "%s, 6666 %s\n", __func__, buf);
  }
  */

  LOGD(TAG "%s, 44444 %s\n", __func__, buf);
  pl = strstr(buf, "Error");
  LOGD(TAG "%s, 55555\n", __func__);
  ret = pclose(stream);

  if (ret == 127)
  {
    LOGD(TAG "%s, bad command\n", __func__);
  }
  else if (ret == -1)
  {
    LOGD(TAG "%s, failed to get child status:%s\n", __func__, strerror(errno));
  }
  else
  {
    print_wait_exit(ret);
  }

  if (pl != NULL)
  {
    nfc->mod->test_result = FTM_TEST_FAIL;
  }
  else
  {
    nfc->mod->test_result = FTM_TEST_PASS;
  }
  exit = true;
  break;
  //wangfeng add
  ```
