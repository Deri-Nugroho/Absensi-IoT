import json
import boto3

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('BukuAbsenTable')

def handler(event, context):
    response = table.scan()
    items = response['Items']
    return {
        'statusCode': 200,
        'headers': {'Access-Control-Allow-Origin': '*'},
        'body': json.dumps(items)
    }
