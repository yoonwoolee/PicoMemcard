from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time

d = webdriver.Chrome('c:/chromedriver/chromedriver.exe')
f = open('titleid_name.txt', 'w', encoding='utf-8')
for i in range(1, 111):
     d.get('https://serialstation.com/titles/?systems=9dbb51a8-eec7-40c2-93b6-fd470996e5ca&page=' + str(i))
     WebDriverWait(d, timeout=40).until(EC.presence_of_element_located((By.TAG_NAME, "tbody")))
     xpath = '//*[@id="filter-data-wrapper"]/div[2]/div/div/div/div/table/tbody'
     e=d.find_element("xpath", xpath)
     f.write(e.text)
f.close()
