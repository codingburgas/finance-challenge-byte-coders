import smtplib
import sys
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

# Email credentials
email = 'lopipas14@gmail.com'
password = 'sjnr hfix mqom rpnt'

# Ensure at least two command-line arguments are provided (recipient email and verification code)
if len(sys.argv) < 3:
    print("Usage: python email.py <recipient_email> <verification_code>")
    sys.exit(1)

# Recipient email from command-line arguments
toaddrs = sys.argv[1]

# Verification code from command-line arguments
verification_code = sys.argv[2]

# Email message format
subject = 'Byte Coders Verification'
body = f"Byte Coders Verification\n\nYour code is: {verification_code}"

# Function to send an email
def send_email(subject, body, from_email, to_email, smtpserver):
    msg = MIMEMultipart()
    msg['From'] = from_email
    msg['To'] = to_email
    msg['Subject'] = subject
    msg.attach(MIMEText(body, 'plain'))
    smtpserver.sendmail(from_email, to_email, msg.as_string())

try:
    with smtplib.SMTP('smtp.gmail.com', 587) as smtpserver:
        smtpserver.ehlo()
        smtpserver.starttls()
        smtpserver.ehlo()
        smtpserver.login(email, password)

        # Send the verification message
        send_email(subject, body, email, toaddrs, smtpserver)
        print(f"Sent: {body}")
except Exception as e:
    print(f"Error: {e}")
