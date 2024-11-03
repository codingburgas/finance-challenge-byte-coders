import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def Download(recipient, total_income, total_expense, total_budget):
    # Email credentials
    email = 'lopipas14@gmail.com'  # Replace with your email
    password = 'sjnr hfix mqom rpnt'  # Replace with your password (consider using environment variables or secure storage)

    # Email subject
    subject = 'ByteCoders Statistics'

    # Prepare the body of the email
    body = (
        f"ByteCoders Statistics\n\n"
        f"Your income is: {total_income}\n"
        f"Your expense is: {total_expense}\n"
        f"Your budget is: {total_budget}\n\n"
    )

    # Determine conclusion based on budget
    if total_budget > 0 and (total_income / total_budget) < (1 / 0.2):
        conclusion = "Your budget is low compared to your income; we recommend cutting your expenses!"
    else:
        conclusion = "Your budget seems pretty good; nothing to worry about!"

    body += conclusion

    # Create the email message
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

# Example usage:
# Download('user@example.com', 5000.0, 2000.0, 10000.0)
