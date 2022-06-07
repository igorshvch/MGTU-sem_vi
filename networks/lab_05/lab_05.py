from email import encoders
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.mime.text import MIMEText
from email.utils import formatdate
import re
from time import sleep
import smtplib
import ssl

def send_message(from_user="testacc.igorshvch@yandex.ru",
                 to_user="testacc.igorshvch@yandex.ru",
                 post_pass=None,
                 msg_text=None,
                 attach=None,
                 sleep_time=20,
                 repetitions=3):

    if re.match(r"^[A-Za-z0-9\.\+_-]+@[A-Za-z0-9\._-]+\.[a-zA-Z]*$", from_user):
        USER_FROM = from_user
    else:
        raise ValueError("Incorrect sender address!")
    if re.match(r"^[A-Za-z0-9\.\+_-]+@[A-Za-z0-9\._-]+\.[a-zA-Z]*$", to_user):
        TO_USER = to_user
    else:
        raise ValueError("Incorrect recipient address!")

    if not post_pass:
        try:
            with open("static/pass.txt", "r") as fl:
                PASS = fl.read()
        except Exception as e:
            print(e)
            return 1
    else:
        PASS = post_pass
    
    if msg_text:
        TEXT_ADDRESS =  attach
    else:
        TEXT_ADDRESS =  "static/text.txt"
    
    if attach:
        ATTACH_NAME = attach.split("/")[-1]
        ATTACH_ADDRESS =  attach
    else:
        ATTACH_NAME = "universe.jpg"
        ATTACH_ADDRESS =  "static/" + ATTACH_NAME

    SERVER = "smtp.yandex.ru"
    PORT = "465"
    INTR = sleep_time
    REPS = repetitions

    msg = MIMEMultipart()
    msg["From"] = USER_FROM
    msg["To"] = USER_FROM
    msg["Date"] = formatdate(localtime=True)
    msg["Subject"] = "Place of a man in the Universe"

    with open(TEXT_ADDRESS, "r") as fl:
        text = fl.read()

    with open(ATTACH_ADDRESS, "rb") as bin_fl:
        attachment = bin_fl.read()
    msg.attach(MIMEText(text, "plain"))

    p = MIMEBase("application", "octet-stream")
    p.set_payload(attachment)
    encoders.encode_base64(p)
    p.add_header("Content-Disposition", f"attachment; filename={ATTACH_NAME}")
    msg.attach(p)

    mail_text = msg.as_string()

    with smtplib.SMTP_SSL(SERVER, port=PORT, context=ssl.create_default_context()) as post_server:
        post_server.login(USER_FROM, PASS)
        while (REPS):
            print(f"...trying to send for {REPS} time...")
            post_server.sendmail(USER_FROM, TO_USER, msg=mail_text)
            REPS-=1
            sleep(INTR)

if __name__ == "__main__":
    print("\n\tIMPORTANT! Please, choose yandex email account to send emails by this client!\n")
    print("Specify params:")
    send_from = input("Input From-email:\t")
    send_to = input("Input From-email:\t")
    password = input("Input email password:\t")
    msg_text = input("Input email body (press 0 to use default)\t")
    attach = input("Input attached picture address (press 0 to use default)\t")
    sleep_time = int(input("Input sleep time (press 0 to use default - 20 seconds):\t"))
    repetitions = int(input("Input number of repetitions (press 0 to use default - 3 times):\t"))

    if not password:
        print("\n\t\tUsing predifined email password from 'static/pass.txt' file\n")
    if msg_text == '0':
        msg_text = None
    if attach == '0':
        attach = None

    send_message(from_user="testacc.igorshvch@yandex.ru",
                 to_user="testacc.igorshvch@yandex.ru",
                 post_pass=password,
                 msg_text=msg_text,
                 attach= attach,
                 sleep_time= sleep_time if sleep_time else 20,
                 repetitions= repetitions if repetitions else 3)
    