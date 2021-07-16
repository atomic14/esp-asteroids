#include <cstring>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MagneticRotaryEncoder.hpp"

static const uint16_t AS5048A_CLEAR_ERROR_FLAG = 0x0001;
static const uint16_t AS5048A_PROGRAMMING_CONTROL = 0x0003;
static const uint16_t AS5048A_OTP_REGISTER_ZERO_POS_HIGH = 0x0016;
static const uint16_t AS5048A_OTP_REGISTER_ZERO_POS_LOW = 0x0017;
static const uint16_t AS5048A_DIAG_AGC = 0x3FFD;
static const uint16_t AS5048A_MAGNITUDE = 0x3FFE;
static const uint16_t AS5048A_ANGLE = 0x3FFF;

static const uint8_t AS5048A_AGC_FLAG = 0xFF;
static const uint8_t AS5048A_ERROR_PARITY_FLAG = 0x04;
static const uint8_t AS5048A_ERROR_COMMAND_INVALID_FLAG = 0x02;
static const uint8_t AS5048A_ERROR_FRAMING_FLAG = 0x01;

static const uint16_t AS5048A_DIAG_COMP_HIGH = 0x2000;
static const uint16_t AS5048A_DIAG_COMP_LOW = 0x1000;
static const uint16_t AS5048A_DIAG_COF = 0x0800;
static const uint16_t AS5048A_DIAG_OCF = 0x0400;

static const double AS5048A_MAX_VALUE = 16384.0f;

static const char *TAG = "MAG";

uint8_t spiCalcEvenParity(uint16_t value)
{
  uint8_t cnt = 0;

  for (uint8_t i = 0; i < 16; i++)
  {
    if (value & 0x1)
    {
      cnt++;
    }
    value >>= 1;
  }
  return cnt & 0x1;
}

/*
 * Read a register from the sensor
 * Takes the address of the register as an unsigned 16 bit
 * Returns the value of the register
 */
uint16_t MagneticRotaryEncoder::read(uint16_t registerAddress)
{
  uint16_t command = 0x4000; // PAR=0 R/W=R
  command = command | registerAddress;

  //Add a parity bit on the the MSB
  command |= static_cast<uint16_t>(spiCalcEvenParity(command) << 0xF);

  gpio_set_level(cs, 0);

  // send the command
  spi_transaction_t t1;
  memset(&t1, 0, sizeof(t1)); //Zero out the transaction
  t1.length = 16;
  t1.rxlength = 16;
  t1.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
  *((uint16_t *)t1.tx_data) = SPI_SWAP_DATA_TX(command, 16);
  spi_device_polling_transmit(spi, &t1);

  gpio_set_level(cs, 1);

  // is this needed?
  vTaskDelay(5);
  gpio_set_level(cs, 0);
  // dummy send to get the response
  memset(&t1, 0, sizeof(t1)); //Zero out the transaction
  t1.length = 16;
  t1.rxlength = 16;
  t1.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
  spi_device_polling_transmit(spi, &t1);
  gpio_set_level(cs, 1);
  // reconstruct it
  uint16_t response = SPI_SWAP_DATA_RX(*((uint16_t *)t1.rx_data), 16);
  //Check if the error bit is set
  if (response & 0x4000)
  {
    this->error_flag = true;
  }
  else
  {
    this->error_flag = false;
  }
  //Return the data, stripping the parity and error bits
  return response & ~0xC000;
}

void rotary_encoder_reader_task(void *param)
{
  MagneticRotaryEncoder *encoder = static_cast<MagneticRotaryEncoder *>(param);
  int debug_count = 0;
  while (true)
  {
    debug_count++;
    encoder->update_current_angle();
    if (debug_count > 10)
    {
      debug_count = 0;
      if (encoder->error_flag)
      {
        ESP_LOGE(TAG, "%s", encoder->get_and_clear_errors());
      }
      ESP_LOGI(TAG, "%.2f", encoder->get_angle());
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void MagneticRotaryEncoder::update_current_angle()
{
  this->current_angle = this->read_angle();
}

float MagneticRotaryEncoder::read_angle()
{
  uint16_t raw_position = read(AS5048A_ANGLE);
  uint16_t rotation = static_cast<int16_t>(raw_position) - static_cast<int16_t>(this->zero_position);
  float radians = 2 * M_PI * float(rotation) / AS5048A_MAX_VALUE;
  return radians;
}

const char *MagneticRotaryEncoder::get_and_clear_errors()
{
  uint16_t error = read(AS5048A_CLEAR_ERROR_FLAG);
  if (error & AS5048A_ERROR_PARITY_FLAG)
  {
    return "Parity Error";
  }
  if (error & AS5048A_ERROR_COMMAND_INVALID_FLAG)
  {
    return "Command invalid";
  }
  if (error & AS5048A_ERROR_FRAMING_FLAG)
  {
    return "Framing error";
  }
  return "No errors";
}

MagneticRotaryEncoder::MagneticRotaryEncoder(gpio_num_t cs, gpio_num_t clk, gpio_num_t miso, gpio_num_t mosi)
{
  // default zero position
  zero_position = 0;

  // setup the LDAC output
  this->cs = cs;
  gpio_set_direction(cs, GPIO_MODE_OUTPUT);

  // setup SPI output
  esp_err_t ret;
  spi_bus_config_t buscfg = {
      .mosi_io_num = mosi,
      .miso_io_num = miso,
      .sclk_io_num = clk,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 0,
      .flags = SPICOMMON_BUSFLAG_MASTER};
  spi_device_interface_config_t devcfg = {
      .command_bits = 0, ///< Default amount of bits in command phase (0-16), used when ``SPI_TRANS_VARIABLE_CMD`` is not used, otherwise ignored.
      .address_bits = 0, ///< Default amount of bits in address phase (0-64), used when ``SPI_TRANS_VARIABLE_ADDR`` is not used, otherwise ignored.
      .dummy_bits = 0,   ///< Amount of dummy bits to insert between address and data phase
      .mode = 1,         //SPI mode 0
      .clock_speed_hz = 3000000,
      .spics_io_num = -1, //CS pin
      .flags = SPI_DEVICE_NO_DUMMY,
      .queue_size = 2};
  //Initialize the SPI bus
  ret = spi_bus_initialize(VSPI_HOST, &buscfg, 2);
  assert(ret == ESP_OK);
  //Attach the SPI device
  ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi);
  assert(ret == ESP_OK);

  // make sure to start the task on CPU 1
  TaskHandle_t timer_setup_handle;
  xTaskCreatePinnedToCore(rotary_encoder_reader_task, "Rotary Encoder Task", 4096, this, 0, &timer_setup_handle, 0);
}

MagneticRotaryEncoder::~MagneticRotaryEncoder()
{
}
