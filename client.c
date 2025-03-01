// BORACIU IONUT SORIN - 325CA
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

/*
	Reads a string of 100 safely, and checks if an error occured
*/

int readAndCheck(char *s,char *cookie,char *access_token) {
	if(fgets(s,100,stdin) < 0) {
		printf("Reading failed at get_book_id\n");
	}
	if(s[strlen(s)-1] == '\n') {
		s[strlen(s)-1] = '\0';
		return 1;
	}
	return 0;
}

/*
	Creates a json_object containing a password and username
*/

char *createUsPass(char *username, char *pass) {
	JSON_Value *json_file = json_value_init_object();
	JSON_Object *json_object = json_value_get_object(json_file);
	json_object_set_string(json_object, "username", username);
	json_object_set_string(json_object, "password", pass);
	
	char *json_string = json_serialize_to_string_pretty(json_file);
	return json_string;
}

/*
	Registeres a new user if it wasnt already taken
	Or prints an error if it cannot make a new user
*/

void registerUser(char *host, char *url, char *type_message) {

	// takes the input from the user
	char username[100];
	char pass[100];
	printf("username=");
	fgets(username,100,stdin);
	username[strlen(username)-1] = '\0';
 	printf("password=");
	fgets(pass,100,stdin);
	pass[strlen(pass)-1] = '\0';

	if(strchr(username,' ') != NULL || strchr(pass,' ') != NULL) {
		printf("No spaces allowed!!!\n");
		return;
	}

	//  prepares the data to be send
	char *json_data[] = {createUsPass(username,pass)};
	char *message = compute_post_request(host, url, type_message, json_data, 1, NULL, 0, NULL);
	int sockfd = open_connection("34.246.184.49", 8080, AF_INET,SOCK_STREAM, 0);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	// checks if an error occoured
	if(strstr(response, "error") != NULL) {
		printf("200 - NOT OK - Utilizator deja înregistrat!\n");
	} else {
		printf("200 - OK - Utilizator înregistrat cu succes!\n");
	}

	close(sockfd);
}

int main (int argc, char *argv[]) {
	// initialized the variabiles
	char *host = "34.246.184.49:8080";
	char *url = "/api/v1/tema/auth/register";
	char *type_message = "application/json";
	char username[100];
	char pass[100];
	char *cookie = malloc(2000);
	char *access_token = malloc(2000);
	int log = 0;

	char command[50];
	fgets(command,50,stdin);
	while (strcmp(command, "exit\n") != 0) {
		if (strcmp(command, "register\n") == 0) {
			registerUser(host,url,type_message);
	
		} else if (strcmp(command,"login\n") == 0) {

			printf("username=");
			fgets(username,100,stdin);
			username[strlen(username)-1] = '\0';
 			printf("password=");
			fgets(pass,100,stdin);
			pass[strlen(pass)-1] = '\0';

			if(strchr(username,' ') != NULL || strchr(pass,' ') != NULL) {
				printf("No spaces allowed!!!\n");
				fgets(command, 50,stdin);
				continue;
			}
			// prepares the json message and sends it
			char *json_data[] = {createUsPass(username,pass)};
			char *message = compute_post_request(host, "/api/v1/tema/auth/login", type_message, json_data, 1, NULL, 0,NULL);
			int sockfd = open_connection("34.246.184.49", 8080, AF_INET,SOCK_STREAM, 0);
			send_to_server(sockfd, message);
			char *response = receive_from_server(sockfd);

			// checks if an error occoured
			if (strstr(response, "error") != NULL) {
				printf("200 - NOT OK - Failed to register!\n");
				char *reasson_failed = strtok(strstr(response,"error"),"\n");
				if (strstr(reasson_failed,"Credentials are not good") != NULL) {
					printf("Invalid username or password, try again\n");
				} else if (strstr(reasson_failed,"No account") != NULL) {
					printf("No such account, you have to register it first\n");
				} else {
					printf("Unexpected error, please, try again\n");
				}
			} else {
				// if the message received is as expected save the auth cookie
				printf("200 - OK - Succesefully connected!\n");
				log = 1;
				memcpy(cookie, strtok(strstr(response, "Set-Cookie"), "\n") + 12, 2000);
				cookie = strtok(cookie, ";");
			}
			close(sockfd);

		} else if (strcmp(command, "enter_library\n") == 0) {
			if (log == 0) {
				printf("No user connected, establish a login session first\n");
				fgets(command, 50, stdin);
				continue;
			} else {
				char *cookies[] = {cookie};
				// sends the message using the login token in order to retrive the auth token
				// in order to use the library
				char *request = compute_get_request(host, "/api/v1/tema/library/access", NULL, cookies, 1, NULL, 0);
				int sockfd = open_connection("34.246.184.49", 8080, AF_INET,SOCK_STREAM, 0);
				send_to_server(sockfd, request);
				char *response = receive_from_server(sockfd);
				if (strstr(response, "error") != NULL) {
					printf("Unexpected error, invalid user\nTry to login again\n");
				} else {
					// if the auth token was received print a successful message
					log++;
					char *trim_res = strstr(response, "token")+8;
					trim_res[strlen(trim_res)-2] = '\0';
					memcpy(access_token, trim_res, 2000);
					printf("Succesfully obtained the access token\n");
				}
				close(sockfd);
			}
		} else if (strcmp(command, "get_books\n") == 0) {
			// check that the user is indeed connected and has obtained the auth token
			if (log == 2) {
				// open the session to the server and request the data of the books
				char *request = compute_get_request(host, "/api/v1/tema/library/books", NULL, NULL, 0, access_token, 0);
				int sockfd = open_connection("34.246.184.49", 8080, AF_INET,SOCK_STREAM, 0);
				send_to_server(sockfd, request);
				char *response = receive_from_server(sockfd);
				char last[1000];
				char *tok = strtok(response, "\n");
				while (tok != NULL) { 
					strcpy(last, tok);
					tok = strtok(NULL, "\n");
				}
				if (strstr(last, "error") == NULL) {
					// if no error occoured print the books received from the server
					JSON_Value *json_file = json_parse_string(last);
					char *json_string = json_serialize_to_string_pretty(json_file);
					printf("%s\n", json_string);
				} else {
					// print an empty json if no books were found
					JSON_Value *json_value = json_value_init_object();
					char *serialized_string = json_serialize_to_string_pretty(json_value);
					printf("%s\n", serialized_string);
				}
				close(sockfd);
			} else {
				if(log == 0) {
					printf("No user logged int");
				} else {
					printf("No access token\n");
				}
			}
		} else if (strcmp(command, "add_book\n") == 0) {
			// make sure that the user is connected and it has access to the library
			if (log == 2) {
				char title[100], author[100], genre[50], publisher[100];
				int number;
				printf("title=");
				// read with fgets every string and check if it was
				// read corectly
				int err = readAndCheck(title, cookie, access_token);
				printf("author=");
				err = readAndCheck(author, cookie, access_token);
				printf("genre=");
				err = readAndCheck(genre, cookie, access_token);
				printf("page_count=");
				char number_s[100];
				err = readAndCheck(number_s,cookie,access_token);
				number = atoi(number_s);

				printf("publisher=");
				err = readAndCheck(publisher, cookie, access_token);

				if(strlen(title) == 0 || strlen(author) == 0 || strlen(genre) == 0 || strlen(publisher) == 0 || number <= 0) {
					printf("Error in reading, try again\n");
					fgets(command, 50,stdin);
					continue;
				}

				// prepare the json data for sending it to the server
				// as to create a new book in the library
				JSON_Value *json_file = json_value_init_object();
				JSON_Object *json_object = json_value_get_object(json_file);
				json_object_set_string(json_object, "title", title);
				json_object_set_string(json_object, "author", author);
				json_object_set_string(json_object, "genre", genre);
				json_object_set_number(json_object,"page_count",number);
				json_object_set_string(json_object, "publisher", publisher);
				char *json_string = json_serialize_to_string_pretty(json_file);
				char *json_data[] = {json_string};

				// send the data and check if it was created or an error occoured
				char *message = compute_post_request(host, "/api/v1/tema/library/books", type_message, json_data, 1, NULL, 0, access_token);
				int sockfd = open_connection("34.246.184.49", 8080, AF_INET,SOCK_STREAM, 0);
				send_to_server(sockfd, message);
				char *response = receive_from_server(sockfd);
				if (strstr(response,"error") != NULL) {
					printf("Error occcured, try again\n");
				} else {
					printf("Book %s added successfuly\n",title);
				}
			} else {
				if(log == 0) {
					printf("No user logged int");
				} else {
					printf("No access token\n");
				}
			}

		} else if (strcmp(command, "get_book\n") == 0 || strncmp(command, "get_book",8) == 0) {
			if (log == 2) {
				int id;
				char id_buff[20];
				if (strcmp(command,"get_book\n") == 0) {
					printf("id=");
					scanf("%d", &id);
					// convert the integer id into a string
					// to concatenate it
					sprintf(id_buff, "%d", id);
				} else {
					command[strlen(command) - 1] = '\0';
					strcpy(id_buff, command + 9);
				}
				char s[100] = "/api/v1/tema/library/books/";
				strcat(s, id_buff);
				// send the request
				char *request = compute_get_request(host, s, NULL, NULL, 0, access_token, 0);
				int sockfd = open_connection("34.246.184.49", 8080, AF_INET,SOCK_STREAM, 0);
				send_to_server(sockfd, request);
				char *response = receive_from_server(sockfd);

				if (strstr(response, "error") == NULL) {
					// if no error occoured print the json
					// containing the data of the book
					char *val_2 = malloc(1000);
					char *p = strtok(response, "\n");
					while (p != NULL) {
						strcpy(val_2, p);
						p = strtok(NULL, "\n");
					}
					JSON_Value *json_file = json_parse_string(val_2);
					char *json_string = json_serialize_to_string_pretty(json_file);
					printf("%s\n", json_string);
					free(val_2);
				} else {
					char error[100];
					strcpy(error, strstr(response, "error") + 8);
					error[strlen(error) - 2] = '\0';
					printf("%s\n", error);
				}
			}
			else {
				if(log == 0) {
					printf("No user logged int");
				} else {
					printf("No access token\n");
				}
			}

		} else if (strcmp(command, "delete_book\n") == 0 || strncmp(command, "delete_book", 11) == 0) {
			if (log == 2) {
				int id;
				char id_buff[20];
				// delete works in the same way as get_book
				// the only difference being that now we write
				// delete into the request instead of get
				if (strcmp(command, "delete_book\n") == 0) {
					printf("id=");
					scanf("%d", &id);
					sprintf(id_buff, "%d", id);
				} else {
					command[strlen(command) - 1] = '\0';
					strcpy(id_buff, command + 12);
				}
				char s[100] = "/api/v1/tema/library/books/";
				strcat(s, id_buff);
				char *request = compute_get_request(host, s, NULL, NULL, 0, access_token, 1);
				int sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
				send_to_server(sockfd,request);
				char *response = receive_from_server(sockfd);
				if (strstr(response,"error") != NULL) {
					char s[100];
					strcpy(s, strstr(response, "error") + 8);
					s[strlen(s) - 2] = '\0';
					printf("%s\n", s);
				} else {
					printf("Book %s successfully deleted\n", id_buff);
				}

			} else {
				if(log == 0) {
					printf("No user logged int");
				} else {
					printf("No access token\n");
				}
			}
		} else if (strcmp(command, "logout\n") == 0) {
			if (log >= 1) {
				char *cookies[] = {cookie};
				// send the request announcing the host that we will disconnect
				char *request = compute_get_request(host, "/api/v1/tema/auth/logout", NULL, cookies, 1, NULL, 0);
				int sockfd = open_connection("34.246.184.49", 8080, AF_INET,SOCK_STREAM, 0);
				send_to_server(sockfd, request);
				char *response = receive_from_server(sockfd);
				// remove the data
				strcpy(cookie, "");
				strcpy(access_token, "");
				// we are no longer connected
				log = 0;
				if (strstr(response, "error") != NULL) {
					printf("Error!!\n");
				} else {
					printf("User logged off successfully\n");
				}
			} else {
				printf("No user connected\n");
			}
		}
		fgets(command, 50,stdin);
	}

	free(access_token);
	free(cookie);
	return 0;
}
