#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Function to send a welcome email using Brevo API
void send_welcome_email(const char *email, const char *first_name) {
    CURL *curl;
    CURLcode res;

    // Your Brevo API Key
    const char *api_key = "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";

    // API Endpoint
    const char *api_url = "https://api.brevo.com/v3/smtp/email";

    // JSON Payload
    char json_data[1024];
    snprintf(json_data, sizeof(json_data),
             "{"
             "\"sender\":{\"name\":\"Your Service\",\"email\":\"shadowonline3574+brevo@gmail.com\"},"
             "\"to\":[{\"email\":\"%s\",\"name\":\"%s\"}],"
             "\"subject\":\"Welcome to Our Service\","
             "\"htmlContent\":\"<html><body><h1>Welcome, %s!</h1><p>Thank you for registering with us.</p></body></html>\""
             "}",
             email, first_name, first_name);

    // Initialize cURL
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "api-key: %s", api_key);
        headers = curl_slist_append(headers, auth_header);

        // Set cURL options
        curl_easy_setopt(curl, CURLOPT_URL, api_url);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
        } else {
            printf("Welcome email sent successfully to %s!\n", email);
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

int main() {
    char first_name[50], last_name[50], mobile[15], gender[10], address[100], email[50];
    int age;

    // Collect user details
    printf("Enter First Name: ");
    scanf("%49s", first_name);

    printf("Enter Last Name: ");
    scanf("%49s", last_name);

    printf("Enter Mobile Number: ");
    scanf("%14s", mobile);

    printf("Enter Gender: ");
    scanf("%9s", gender);

    printf("Enter Age: ");
    scanf("%d", &age);

    getchar(); // Clear input buffer
    printf("Enter Address: ");
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = '\0'; // Remove newline character

    printf("Enter Email Address: ");
    scanf("%49s", email);

    // Check if the file exists; if not, create it
    FILE *file = fopen("Userdata.txt", "a");
    if (file == NULL) {
        perror("Error opening Userdata.txt");
        return 1;
    }

    // Append the user data to the file
    fprintf(file, "First Name: %s\nLast Name: %s\nMobile: %s\nGender: %s\nAge: %d\nAddress: %s\nEmail: %s\n\n",
            first_name, last_name, mobile, gender, age, address, email);
    fclose(file);

    printf("Registration successful! Details saved to 'Userdata.txt'.\n");

    // Send a welcome email
    send_welcome_email(email, first_name);

    return 0;
}
