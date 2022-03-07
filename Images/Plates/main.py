import cv2
import easyocr

if __name__ == '__main__':
    reader = easyocr.Reader(['en'], gpu=False)
    img = cv2.imread('118.jpg')
    cv2.imshow('img', img)
    results = reader.readtext(img, detail=1, paragraph=False)

    #print(results)

    for (bbox, text, prob) in results:
        text = "".join([c if ord(c) < 128 else "" for c in text]).strip()

        print(text)

    cv2.waitKey(0)
