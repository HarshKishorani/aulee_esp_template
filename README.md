Before building and flashing any firmware make sure you have ***esp-idf 5.1.1*** installed and esp_aws_iot libraries installed.

To Install esp_aws_iot libraries, go to `[YOUR_ESP_IDF_PATH]/components` in cmd and run the following commands to install **esp_aws_iot v3.1.x** as shown [here](https://youtu.be/0Lt-bMbJyKc).

   1. `git clone --recursive https://github.com/espressif/esp-aws-iot`
   2. `cd esp-aws-iot`
   3. `git checkout release/v3.1.x`
   4. `git submodule update --recursive`