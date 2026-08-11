#include "weather.h"

#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"

#include "config/project_config.h"

static const char* TAG = "WEATHER";

#define WEATHER_URL "http://api.openweathermap.org/data/2.5/weather?q=Cheboksary&appid=0c2655e65825bf4f5ded333889bd03d8&units=metric"

//-----------------------------------------------------------------------------------------
bool weather_update_sync( void )
{
	ESP_LOGI( TAG, "Starting weather update..." );

	// Буфер для скачивания ответа
	int buffer_size = 2048;
	char* local_response_buffer = ( char* )malloc( buffer_size );

	if ( !local_response_buffer )
	{
		ESP_LOGE( TAG, "Failed to allocate memory for HTTP buffer" );
		return false;
	}

	esp_http_client_config_t config =
	{
		.url = WEATHER_URL,
		.method = HTTP_METHOD_GET,
		.timeout_ms = 10000, // 10 секунд на ответ
	};

	bool res = false;

	// Делаем 3 попытки скачивания (защита от кратковременных падений DNS/Wi-Fi)
	for ( int retry = 0; retry < 3; retry++ )
	{
		esp_http_client_handle_t client = esp_http_client_init( &config );
		esp_err_t err = esp_http_client_open( client, 0 );

		if ( err == ESP_OK )
		{
			esp_http_client_fetch_headers( client );
			int read_len = esp_http_client_read_response( client, local_response_buffer, buffer_size - 1 );

			if ( read_len >= 0 )
			{
				local_response_buffer[read_len] = '\0';
				ESP_LOGI( TAG, "Successfully downloaded weather data (%d bytes)", read_len );

				cJSON* root = cJSON_Parse( local_response_buffer );
				if ( root )
				{
					int8_t temp_val = 0;
					char icon_str[8] = { 0 };
					char desc_str[32] = { 0 };

					cJSON* main_obj = cJSON_GetObjectItem( root, "main" );
					if ( main_obj )
					{
						cJSON* temp_obj = cJSON_GetObjectItem( main_obj, "temp" );
						if ( temp_obj )
							temp_val = ( int8_t )temp_obj->valuedouble;
					}

					cJSON* weather_arr = cJSON_GetObjectItem( root, "weather" );
					if ( weather_arr )
					{
						cJSON* weather_item = cJSON_GetArrayItem( weather_arr, 0 );
						if ( weather_item )
						{
							cJSON* icon_obj = cJSON_GetObjectItem( weather_item, "icon" );
							if ( icon_obj && icon_obj->valuestring )
								strncpy( icon_str, icon_obj->valuestring, sizeof( icon_str ) - 1 );

							cJSON* desc_obj = cJSON_GetObjectItem( weather_item, "description" );
							if ( desc_obj && desc_obj->valuestring )
								strncpy( desc_str, desc_obj->valuestring, sizeof( desc_str ) - 1 );
						}
					}

					ESP_LOGI( TAG, "Weather parsed -> Temp: %d°C, Icon: %s, Desc: %s", temp_val, icon_str, desc_str );
					project_config_set_weather( temp_val, icon_str, desc_str );
					cJSON_Delete( root );

					res = true; // Успешно распарсили!
				}
				else
				{
					ESP_LOGE( TAG, "Failed to parse JSON" );
				}
			}
			else
			{
				ESP_LOGE( TAG, "Failed to read HTTP response" );
			}

			esp_http_client_cleanup( client );

			if ( res )
				break; // Если все прошло успешно - выходим из цикла попыток
		}
		else
		{
			ESP_LOGE( TAG, "Failed to open HTTP connection: %s", esp_err_to_name( err ) );
			esp_http_client_cleanup( client );

			ESP_LOGI( TAG, "Retrying weather update in 2 seconds... (%d/3)", retry + 1 );
			vTaskDelay( pdMS_TO_TICKS( 2000 ) ); // Ждем 2 секунды перед новой попыткой
		}
	}

	free( local_response_buffer );
	ESP_LOGI( TAG, "Weather update sync finished." );

	return res;
}

//-----------------------------------------------------------------------------------------