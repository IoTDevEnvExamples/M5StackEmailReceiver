using System;
using System.Net;
using System.Net.Http.Headers;
using System.Net.Http.Formatting;
using Microsoft.Azure.Devices;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using Newtonsoft.Json;
using HtmlAgilityPack;

static RegistryManager registryManager;
static ServiceClient serviceClient;
static string connectionString = Environment.GetEnvironmentVariable("iotHubConnectionString");

// Modify the device name for your environment
static string deviceName = "t-yiwche-test2-iothub-device";
public class Email
{
    public string Body { get; set; }
    public string From { get; set; }
    public string Subject { get; set; }
}

public static async Task<HttpResponseMessage> Run(HttpRequestMessage req, TraceWriter log)
{
    HttpResponseMessage response;
    registryManager = RegistryManager.CreateFromConnectionString(connectionString);
    serviceClient = ServiceClient.CreateFromConnectionString(connectionString);
    // parse query parameter
    string action = req.GetQueryNameValuePairs()
        .FirstOrDefault(q => string.Compare(q.Key, "action", true) == 0)
        .Value;
    if (action == "new")
    {
        var content = req.Content;
        var bodyjson = JsonConvert.DeserializeObject<Email>(content.ReadAsStringAsync().Result);
        HtmlDocument htmldoc = new HtmlDocument();
        htmldoc.LoadHtml(bodyjson.Body);
        bodyjson.Body = htmldoc.DocumentNode.InnerText;
        var message = new Message(Encoding.ASCII.GetBytes(JsonConvert.SerializeObject(bodyjson)));
        await serviceClient.SendAsync(deviceName, message);
        response = new HttpResponseMessage(HttpStatusCode.OK);
    }
    else
    {
        response = new HttpResponseMessage(HttpStatusCode.BadRequest);
    }

    return response;
}
