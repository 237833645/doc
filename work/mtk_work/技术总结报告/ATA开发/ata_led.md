# ATA LED配置及开发

## 开发背景

LED有各种灯,3色灯、闪关灯红外灯、按键灯等,都统一使用led测试项来测试,需要添加AfxMessageBox来人工判断

## 客制化内容

- ata代码路径添加AfxMessageBox相关代码:
- 工厂测试模式添加相关led的节点测试功能

## 关健代码相关修改

- ATA代码修改如下

```c++
// ATA_DLL\TestItem\src\ATA_DLL_TestItem_LED.cpp

E_ATADLL_RESULT CTestItem_LED::StartTest()
{
  char led_state[LED_TEST_TIMES][MAX_LED_SUPPORT_NUM];
  ATResult atr;
  E_ATADLL_RESULT testResultCode;

  ClearATResPool();
  test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED, 10);
  OpenLED();
  AddLog("Open LED.");

#if 1 // wangfeng add for led test AfxMessageBox check
  int check =
      AfxMessageBox("LED显示是否正常?", MB_ICONQUESTION | MB_YESNO, 0);
  if (check == IDYES)
  {
    AddLog("[OK] LCM test ok from target.");
    //test_handle->m_testCNF->bLCMDetected = true;
    testResultCode = E_ATADLL_RESULT_SUCCESS;
    test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED,
                                           30);
    Sleep(1000);
    CloseLED();
    Sleep(1000);
    return testResultCode;
  }
  else if (check == IDNO)
  {
    AddLog("[LCM fail] LCM test fail.");
    test_handle->m_testCNF->bLCMDetected = false;
    testResultCode = E_ATADLL_RESULT_FAILED;
    test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED,
                                           30);
    Sleep(1000);
    CloseLED();
    Sleep(1000);
    return testResultCode;
  }
#endif // wangfeng add for led test AfxMessageBox check

  test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED, 30);
  Sleep(2000);

#ifndef INTERNEL_DEBUG_VERSION
  // Measure and record LED state
  /**
    if (E_ATADLL_RESULT_SUCCESS == MeasureLEDState (led_state)) {
      sprintf (test_handle->m_testCNF->led_cnf.state, led_state);
    }
    **/

  // Measure and record LED state
  int index = 0;
  AddLog("[TestBox]Measure LED state %d times for stability.", LED_TEST_TIMES);
  for (index = 0; index < LED_TEST_TIMES; index++) // Measure LED_TEST_TIMES times
  {
    if (E_ATADLL_RESULT_SUCCESS == MeasureLEDState(led_state[index]))
    {
      AddLog("[%d] LED State: %s", index + 1, led_state[index]);
    }
    else
    {
      AddLog("[ERROR][%d] measure LED State fail", index + 1);
    }
  }

  int ledNumber[LED_TEST_TIMES] = {0};
  for (index = 0; index < LED_TEST_TIMES; index++) // Compute LED number for each test time
  {
    ledNumber[index] = 0;
    for (int ledIndex = 0; ledIndex < strlen(led_state[index]); ledIndex++)
    {
      if (ledIndex >= MAX_LED_SUPPORT_NUM)
        break;
      if (led_state[index][ledIndex] == '0')
      {
        ledNumber[index]++;
      }
    }
  }

  int choosedIndex = 0;
  int maxLedNum = -1;
  for (index = 0; index < LED_TEST_TIMES; index++) // choose the most proper test result ( with more led number )
  {
    if (maxLedNum < ledNumber[index])
    {
      maxLedNum = ledNumber[index];
      choosedIndex = index;
    }
  }

  sprintf(test_handle->m_testCNF->led_cnf.state, led_state[choosedIndex]);
  AddLog("[Time%d] is the most proper test result: %s", choosedIndex + 1, led_state[choosedIndex]);
#endif
  test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED, 50);

  // Close LED
  testResultCode = CloseLED();
  if (E_ATADLL_RESULT_SUCCESS != testResultCode)
  {
    test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED, 100);
    return testResultCode;
  }
  AddLog("Close LED successful.");
  test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED, 80);
  Sleep(1500);

#ifndef INTERNEL_DEBUG_VERSION
  // Measure LED state ( LED state should be all closed
  if (E_ATADLL_RESULT_SUCCESS == MeasureLEDState(led_state[0]))
  {
    if (strstr(led_state[0], "0") == NULL)
    {
      AddLog("[OK] LED state ok after cloing LED: %s.", led_state[0]);
    }
    else
    {
      AddLog("[TestBox ERROR] LED state error after cloing LED: %s.", led_state[0]);
      return E_ATADLL_RESULT_BOX_FAILED;
    }
  }
  else
  {
    return E_ATADLL_RESULT_BOX_FAILED;
  }
#endif
  test_handle->m_commonCFG.cbTestProgess(test_handle->handle_index, E_LED, 100);

  // process addional return string "not test"
  //GetATResponse (atr);mingxue remove

  return E_ATADLL_RESULT_SUCCESS;
}
```

- factory代码修改如下

```c

```

## 代码移植包供参考

## 总结
