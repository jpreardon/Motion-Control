# stockIndexService.rb
require 'sinatra'
require "sinatra/reloader"
require "net/http"
require "uri"
require 'rexml/document'

get '/data' do
  
  # TODO: Check the device ID key, make sure it is a known device, otherwise, error!
  
  # TODO: Return the "chart" as the last five days worth of data expressed as point between 0 and 100
  
  # Fetch data from the file
  dataArray = Array.new
  outputArray = Array.new
  File.open('datalog.txt') do |file|
    while line = file.gets
      dataArray.push(line.split(',')[1].strip)
    end
  end
  
  # Turn it into a "chart" (all values between 0 and 100)
  # (value - minimum)/maximum-minimum)*100
  dataArray.each do |value|
    outputArray.push(((Float(value) - Float(dataArray.min))/(Float(dataArray.max) - Float(dataArray.min)))*100)
  end
  
  # Return the values
  "#{outputArray[0].to_i},#{outputArray[1].to_i},#{outputArray[2].to_i},#{outputArray[3].to_i},#{outputArray[4].to_i}"
  
  
  # TODO: Log the API hit. Store the time and the device ID.
  
  # TODO: Check to see if we need to get data.
  
  # Grab the Last Trade Price from Yahoo!
  #uri = URI.parse("http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20yahoo.finance.quotes%20where%20symbol%20in%20(%22%5EGSPC%22)&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys")
  #xml_data = Net::HTTP.get_response(uri).body
  #REXML::Document.new(xml_data).root.elements["results/quote/LastTradePriceOnly"]
  
  
  # TODO: When we get a new value, we'll stuff it into the data file as the last item
  
  # TODO: We'll need to figure out when to close the day. We can probably just do this at 17:00 ET or something.
  
end