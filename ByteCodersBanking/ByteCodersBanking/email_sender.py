import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def send_email(recipient, verification_code):
    # Email credentials
    email = 'lopipas14@gmail.com'
    password = 'sjnr hfix mqom rpnt'
    
    # Email messages
    subject = 'Your Verification Code'
    body = f'Your verification code is: {verification_code}'

    # Create the message
    msg = MIMEMultipart()
    msg['From'] = email
    msg['To'] = recipient
    msg['Subject'] = subject
    msg.attach(MIMEText(body, 'plain'))

    # Send the email
    try:
        with smtplib.SMTP('smtp.gmail.com', 587) as server:
            server.ehlo()
            server.starttls()
            server.ehlo()
            server.login(email, password)
            server.sendmail(email, recipient, msg.as_string())
            print(f"Email sent to {recipient}")
    except Exception as e:
        print(f"Error sending email: {e}")