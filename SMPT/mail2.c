#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Function to send an anonymous email
void send_anonymous_email(const char *receiver_email, const char *message) {
    CURL *curl;
    CURLcode res;

    // Your Brevo API key
    const char *api_key = "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";

    // Initialize cURL
    curl = curl_easy_init();
    if (curl) {
        // Set Brevo API URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.brevo.com/v3/smtp/email");

        // Prepare the JSON payload
        char json_payload[1024];
        snprintf(json_payload, sizeof(json_payload),
                 "{"
                 "\"sender\":{\"name\":\"Anonymous\",\"email\":\"anonymous@no-reply.com\"},"
                 "\"to\":[{\"email\":\"%s\"}],"
                 "\"subject\":\"Anonymous Message\","
                 "\"htmlContent\":\"<p>%s</p>\""
                 "}",
                 receiver_email, message);

        // Set HTTP headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "api-key: %s", api_key);
        headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST fields
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
        } else {
            printf("Anonymous email sent successfully to %s!\n", receiver_email);
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Failed to initialize cURL.\n");
    }
}

int main() {
    char receiver_email[256];
    char message[1024];

    // Get the recipient's email
    printf("Enter the recipient's email address: ");
    fgets(receiver_email, sizeof(receiver_email), stdin);
    receiver_email[strcspn(receiver_email, "\n")] = '\0'; // Remove newline character

    // Get the message content
    printf("Enter your anonymous message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0'; // Remove newline character

    // Send the anonymous email
    send_anonymous_email(receiver_email, message);

    return 0;
}
