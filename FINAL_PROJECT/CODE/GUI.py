import tkinter as tk
from tkinter import filedialog
import cv2
import PIL.Image as Image 
from PIL import ImageTk
import os

import final_1
import final_2
import final_3

def selectPath():
    filepath =  filedialog.askopenfilename(initialdir = "/",title = "Select file",filetypes = (("jpeg files","*.jpg"), ("png files","*.png"), ("gif files","*.gif"),("all files","*.*")))
    print(filepath)
    path.set(filepath)
    # img_path = askdirectory(title = "Select file",filetypes = (("jpeg files","*.jpg"), ("png files","*.png"), ("gif files","*.gif"), ("all files","*.*")))
    img = cv2.imread(filepath)
    rows,cols,_=img.shape
    if cols/rows < 600/400:
        scale = 400/rows
    else:
        scale = 600/cols
    img = cv2.resize(img,(int(cols*scale),int(rows*scale)))
    #resize前後大小
    # print(cols,rows)
    # print(int(cols*scale),int(rows*scale))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img = ImageTk.PhotoImage(Image.fromarray(img))
    pictureFrame.create_image(300,200,anchor = 'center', image = img)
    pictureFrame.img = img

def project_1():
    filepath=path.get()
    speed = value.get()
    if os.path.isfile(filepath):
        img = cv2.imread(filepath)
        final_1.main1(img,int(speed))
        
def project_2():
    filepath=path.get()
    if os.path.isfile(filepath):
        img = cv2.imread(filepath)
        final_2.main2(img)
        
def project_3():
    filepath=path.get()
    speed = value.get()
    if os.path.isfile(filepath):
        img = cv2.imread(filepath)
        final_3.main3(img,int(speed))

    
#GUI建立
window=tk.Tk()
window.wm_title("動畫模擬系統")


w=1000
h=430
window.geometry("{}x{}".format(w, h))
window.configure(bg='#F8F8FF')

#載入圖片
tk.Label(window,text = "目標路徑:",bg = '#006400',fg = 'white').place(relx=0.03,rely=0.1,anchor='nw')
path = tk.StringVar()
tk.Entry(window, textvariable = path).place(relx=0.1,rely=0.1,anchor='nw')
tk.Button(window, text = "路徑選擇",bg = '#006400',fg = 'white',
                         command = lambda:selectPath()).place(relx=0.25,rely=0.1,anchor='nw')

#速度條
value = tk.StringVar()
scale = tk.Scale(window, from_=0, to=500, orient="horizontal",variable=value).place(relx=0.03,rely=0.8,width=200,heigh=50,anchor='nw')
value.set(100)

#圖片欄
pictureFrame=tk.Canvas(window,bg='#333f50')
pictureFrame.place(relx=0.9755,rely=0.97,width=600,heigh=400,anchor='se')

#選擇題號
tk.Button(window, text = "PROJECT 1",bg = '#006400',fg = 'white',activebackground = '#ADFF2F',
                          command=lambda: project_1()).place(relx=0.03,rely=0.3,width=200,heigh=50,anchor='nw')
tk.Button(window, text = "PROJECT 2",bg = '#006400',fg = 'white',activebackground = '#ADFF2F',
                          command=lambda: project_2()).place(relx=0.03,rely=0.45,width=200,heigh=50,anchor='nw')
tk.Button(window, text = "PROJECT 3",bg = '#006400',fg = 'white',activebackground = '#ADFF2F',
                          command=lambda: project_3()).place(relx=0.03,rely=0.6,width=200,heigh=50,anchor='nw')



window.mainloop()
