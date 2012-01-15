# stockIndexService.rb
require 'sinatra'
require "sinatra/reloader"
require "net/http"
require "uri"
require 'rexml/document'

get '/data' do
  
  # TODO: Check the device ID key, make sure it is a known device, otherwise, error!
  
  # TODO: Return the "chart" as the last five days worth of data expressed as point between 0 and 100 Yeah, we're doing this first. The idea being that the device will be asking us for data every 5 minutes or so. The resolution of the device isn't so great that it will matter if the data stale by 5 minutes. This will allow us to return a result immediately, then we can trigger the data acquisition process to actually get the updated data.
  
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
  # TODO: Instead of returning a line of text, we should probably return some XML or something, this seems pretty hacky as it is
  "*#{outputArray[0].to_i},#{outputArray[1].to_i},#{outputArray[2].to_i},#{outputArray[3].to_i},#{outputArray[4].to_i}*"
  
  
  # TODO: Log the API hit. Store the time and the device ID.
  
  # TODO: Check to see if we need to get data. We can change the resolution here if we need to throttle things, for now, we'll see if 4 minutes has passed. We'll also make sure the market could possibly be open (between 9:30 & 16:30 ET). If not, we'll stop. If it has, we'll get thelatest data. It doesn't really matter what we use here, as long as we can get the data. For now, we'll use YQL
  
  # Grab the Last Trade Price from Yahoo!
  #uri = URI.parse("http://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20yahoo.finance.quotes%20where%20symbol%20in%20(%22%5EGSPC%22)&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys")
  #xml_data = Net::HTTP.get_response(uri).body
  #REXML::Document.new(xml_data).root.elements["results/quote/LastTradePriceOnly"]
  
  
  # TODO: When we get a new value, we'll stuff it into the data file as the last item
  
  # TODO: We'll need to figure out when to close the day. We can probably just do this at 17:00 ET or something. Once the day is closed, we don't need to fetch any more data for it.
  
end