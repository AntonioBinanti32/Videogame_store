import json

from flask import Flask, render_template, request, redirect, url_for, session, flash
import requests
import configparser
from datetime import datetime

app = Flask(__name__)
app.secret_key = 'your_secret_key'

config = configparser.ConfigParser()
config.read('config.ini')

backend_url = config.get('backend', 'backend_url')

#TODO: Implementare users.html
#TODO: Implementare sistema notifiche
#TODO: implementare giochi suggeriti


def is_user_logged_in():
    return 'user' in session

def is_admin():
    if session['user'] == 'admin':
            return True
    else:
        return False

def getHeaders():
    user = session.get('user')
    token = session.get('token')

    headers = {
        'Token': token,
        'ActualUser': user
    }

    return headers

def getUserInformation():
    user = session.get('user')
    response_user = requests.get(f'{backend_url}/getUser/{user}', headers=getHeaders())
    return response_user.json()

# Definisci il filtro personalizzato per la formattazione della data
@app.template_filter('dateformat')
def dateformat(value):
    try:
        if isinstance(value, (int, float)):
            # Se il valore è un timestamp Unix, convertilo in datetime
            dt_object = datetime.utcfromtimestamp(value / 1000.0)
        elif isinstance(value, dict) and '$date' in value:
            # Se il valore è un oggetto data MongoDB
            dt_object = datetime.utcfromtimestamp(value['$date'] / 1000.0)
        else:
            raise ValueError("Formato data non riconosciuto")

        # Formatta la data come desiderato (es. "%Y-%m-%d %H:%M:%S")
        formatted_date = dt_object.strftime("%Y-%m-%d %H:%M:%S")
        return formatted_date
    except Exception as e:
        print(f"Errore nella formattazione del timestamp: {e}")
        return "Data non disponibile"

@app.template_filter('dateformat_input')
def dateformat_input(value):
    try:
        if isinstance(value, dict) and '$date' in value:
            # Se il valore è un oggetto data MongoDB
            dt_object = datetime.utcfromtimestamp(value['$date'] / 1000.0)
        else:
            raise ValueError("Formato data non riconosciuto")

        # Formatta la data come richiesto per l'input di tipo 'date'
        formatted_date = dt_object.strftime("%Y-%m-%d")
        return formatted_date
    except Exception as e:
        print(f"Errore nella formattazione del timestamp per input: {e}")
        return ""

@app.route('/')
def home():
    try:
        if not is_user_logged_in():
            return redirect(url_for('login'))

        actualUser = getUserInformation()

        response_games = requests.get(f'{backend_url}/getGames', headers=getHeaders())
        games = response_games.json()

        # Recupera i parametri di ricerca e filtro dalla richiesta
        search_query = request.args.get('search', '')
        selected_genre = request.args.get('genre', '')
        min_price = request.args.get('min_price', '')
        max_price = request.args.get('max_price', '')

        # Filtra i giochi in base alla ricerca
        if search_query:
            games = [game for game in games if search_query.lower() in game['title'].lower()]

        # Filtra i giochi in base al genere
        if selected_genre:
            games = [game for game in games if game['genre'] == selected_genre]

        # Filtra i giochi in base al prezzo
        if min_price:
            games = [game for game in games if game['price'] >= float(min_price)]
        if max_price:
            games = [game for game in games if game['price'] <= float(max_price)]

        # Recupera i generi dei giochi
        genres = list(set(game['genre'] for game in games))

        return render_template('home.html', games=games, genres=genres, actualUser=actualUser, admin=is_admin())
    except requests.exceptions.HTTPError as http_err:
        flash('Login failed: invalid credentials')
    except requests.exceptions.ConnectionError as conn_err:
        flash('Connection error: please try again later')
    except requests.exceptions.Timeout as timeout_err:
        flash('Request timed out: please try again later')
    except requests.exceptions.RequestException as req_err:
        flash('An unexpected error occurred: please try again later')
    return redirect(url_for('login'))


@app.route('/game/<string:gameTitle>')
def game(gameTitle):
    response = requests.get(f'{backend_url}/getGameByTitle/{gameTitle}', headers = getHeaders())
    game = response.json()
    return render_template('game.html', game=game, admin=is_admin())

@app.route('/create_game', methods=['GET', 'POST'])
def create_game():
    if request.method == 'POST':

        data = {
            'title': request.form['title'],
            'genre': request.form['genre'],
            'releaseDate': request.form['releaseDate'],
            'developer': request.form['developer'],
            'price': float(request.form['price']),
            'stock': int(request.form['stock']),
            'description': request.form['description'],
            'imageUrl': request.form['imageUrl']
        }

        # Converti la data di rilascio dal formato 'YYYY-MM-DD' al formato timestamp


        try:
            response = requests.post(f'{backend_url}/addGame', json=data, headers=getHeaders())
            response.raise_for_status()
            flash('Game created successfully!', 'success')
            return redirect(url_for('home'))
        except requests.exceptions.RequestException as e:
            flash(f"Failed to create game: {e}", 'danger')
            return redirect(url_for('create_game'))
    return render_template('create_game.html')


@app.route('/update_game/<string:gameTitle>', methods=['GET'])
def update_game_form(gameTitle):
    if not is_admin():
        flash('Only admin users can update games.', 'danger')
        return redirect(url_for('home'))

    # Recupera i dettagli del gioco per il form di aggiornamento
    response = requests.get(f'{backend_url}/getGameByTitle/{gameTitle}', headers=getHeaders())
    game = response.json()

    return render_template('update_game.html', game=game)


@app.route('/update_game/<string:gameTitle>', methods=['POST'])
def update_game(gameTitle):
    if not is_admin():
        flash('Only admin users can update games.', 'danger')
        return redirect(url_for('home'))

    update_data = {
        'title': gameTitle,
        'newGenre': request.form['newGenre'],
        'newReleaseDate': request.form['newReleaseDate'],
        'newDeveloper': request.form['newDeveloper'],
        'newPrice': float(request.form['newPrice']),
        'newStock': int(request.form['newStock']),
        'newDescription': request.form['newDescription'],
        'newImageUrl': request.form['newImageUrl']
    }

    try:
        response = requests.put(f'{backend_url}/updateGame', json=update_data, headers=getHeaders())
        response.raise_for_status()
        flash('Game updated successfully!', 'success')
    except requests.exceptions.RequestException as e:
        flash(f"Failed to update game: {str(e)}", 'danger')

    return redirect(url_for('game', gameTitle=gameTitle))


@app.route('/delete_game/<string:gameTitle>', methods=['POST'])
def delete_game(gameTitle):
    if not is_admin():
        flash('Only admin users can delete games.', 'danger')
        return redirect(url_for('home'))

    try:
        response = requests.delete(f'{backend_url}/deleteGame/{gameTitle}', headers=getHeaders())
        response.raise_for_status()
        flash('Game deleted successfully!', 'success')
    except requests.exceptions.RequestException as e:
        flash(f"Failed to delete game: {str(e)}", 'danger')

    return redirect(url_for('home'))



@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        credentials = request.form
        try:
            response = requests.post(f'{backend_url}/login', json=credentials)
            response.raise_for_status()  # Questo genera un'eccezione per codici di stato HTTP 4xx/5xx
            session['user'] = response.json()['user']
            session['token'] = response.json()['token']
            user = getUserInformation()
            session['user_image'] = user['image_url']
            return redirect(url_for('home'))
        except requests.exceptions.HTTPError as http_err:
            flash('Login failed: invalid credentials')
        except requests.exceptions.ConnectionError as conn_err:
            flash('Connection error: please try again later')
        except requests.exceptions.Timeout as timeout_err:
            flash('Request timed out: please try again later')
        except requests.exceptions.RequestException as req_err:
            flash('An unexpected error occurred: please try again later')
        return redirect(url_for('login'))
    return render_template('login.html')


@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        user_data = request.form
        try:
            response = requests.post(f'{backend_url}/signup', json=user_data)
            # Questo genera un'eccezione per codici di stato HTTP 4xx/5xx
            response.raise_for_status()
            session['user'] = response.json()['user']
            session['token'] = response.json()['token']
            return redirect(url_for('home'))
        except requests.exceptions.HTTPError as http_err:
            flash('Signup failed: invalid credentials')
        except requests.exceptions.ConnectionError as conn_err:
            flash('Connection error: please try again later')
        except requests.exceptions.Timeout as timeout_err:
            flash('Request timed out: please try again later')
        except requests.exceptions.RequestException as req_err:
            flash('An unexpected error occurred: please try again later')
        return redirect(url_for('signup'))
    return render_template('signup.html')


@app.route('/reviews/<string:gameTitle>', methods=['GET', 'POST'])
def reviews(gameTitle):
    if request.method == 'POST':
        review_text = request.form.get('review')
        rating = int(request.form.get('rating'))
        username = session.get('user')
        review_data = {
            'username': username,
            'gameTitle': gameTitle,
            'reviewText': review_text,
            'rating': rating
        }

        headers = getHeaders()
        try:
            response = requests.post(f'{backend_url}/addReview', json=review_data, headers=headers)
            response.raise_for_status()
            flash('Review added successfully!', 'success')
        except requests.exceptions.RequestException as e:
            flash(f"Failed to add review: you can isert only one review for a game.", 'danger')

        return redirect(url_for('game', gameTitle=gameTitle))

    else:
        # Metodo GET per ottenere le recensioni già presenti
        response = requests.get(f'{backend_url}/getReviewByGame/{gameTitle}')
        reviews = response.json()
        return render_template('game.html', gameTitle=gameTitle, admin=is_admin())
    return render_template('game.html', gameTitle=gameTitle, admin=is_admin())

@app.route('/cart', methods = ['GET'])
def cart():
    try:
        if not is_user_logged_in():
            return redirect(url_for('login'))

        username = session.get('user')
        response = requests.get(f'{backend_url}/getReservations/{username}', headers=getHeaders())
        response.raise_for_status()
        cart_items = response.json()
        return render_template('cart.html', cart_items=cart_items, admin=is_admin())
    except requests.exceptions.HTTPError as http_err:
        flash('Failed to fetch cart: invalid credentials')
    except requests.exceptions.ConnectionError as conn_err:
        flash('Connection error: please try again later')
    except requests.exceptions.Timeout as timeout_err:
        flash('Request timed out: please try again later')
    except requests.exceptions.RequestException as req_err:
        flash('An unexpected error occurred: please try again later')
    return redirect(url_for('home'))

@app.route('/add_to_cart/<string:gameTitle>', methods=['GET', 'POST'])
def add_to_cart(gameTitle):
    """if 'cart' not in session:
        session['cart'] = []
    session['cart'].append(gameTitle)
    return redirect(url_for('cart'))"""
    if not is_user_logged_in():
        return redirect(url_for('login'))

    quantity = int(request.form.get('quantity'))
    username = session.get('user')
    review_data = {
        'username': username,
        'gameTitle': gameTitle,
        'numCopies': quantity
    }
    try:
        response = requests.post(f'{backend_url}/addReservation', headers = getHeaders(), json=review_data) #TODO: da rivedere
        response.raise_for_status()
        flash('Game added successfully to cart!', 'success')
    except requests.exceptions.RequestException as e:
        flash(f"Failed to add game in cart.", 'danger')

    return redirect(url_for('game', gameTitle=gameTitle))


@app.route('/remove_from_cart/<string:reservation_id>', methods=['POST'])
def remove_from_cart(reservation_id):
    try:
        if not is_user_logged_in():
            return redirect(url_for('login'))
        response = requests.delete(f'{backend_url}/deleteReservation/{reservation_id}', headers=getHeaders())
        response.raise_for_status()

        flash('Game removed from cart successfully!', 'success')
    except requests.exceptions.HTTPError as http_err:
        flash('Failed to remove game from cart: invalid credentials', 'danger')
    except requests.exceptions.ConnectionError as conn_err:
        flash('Connection error: please try again later', 'danger')
    except requests.exceptions.Timeout as timeout_err:
        flash('Request timed out: please try again later', 'danger')
    except requests.exceptions.RequestException as req_err:
        flash('An unexpected error occurred: please try again later', 'danger')

    return redirect(url_for('cart'))

@app.route('/checkout', methods=['GET', 'POST'])
def checkout():
    try:
        if not is_user_logged_in():
            return redirect(url_for('login'))

        if request.method == 'POST':
            cart_items = request.form.get('cart_items')
            if cart_items:
                cart_items = json.loads(cart_items)

            if not cart_items:
                flash('Your cart is empty.', 'danger')
                return redirect(url_for('cart'))

            username = session.get('user')
            # Loop through each item in the cart and create a purchase request
            for item in cart_items:
                purchase_data = {
                    'username': username,
                    'gameTitle': item['game_title'],
                    'numCopies': item['num_copies']
                }
                response = requests.post(f'{backend_url}/addPurchase', json=purchase_data, headers=getHeaders())
                response.raise_for_status()

                # After successfully adding the purchase, remove the item from the cart
                reservation_id = item['_id']['$oid']
                remove_response = requests.delete(f'{backend_url}/deleteReservation/{reservation_id}', headers=getHeaders())
                remove_response.raise_for_status()

            flash('Order placed successfully!', 'success')
            return redirect(url_for('checkout'))

        else:  # GET request
            username = session.get('user')
            response = requests.get(f'{backend_url}/getPurchases/{username}', headers=getHeaders())
            response.raise_for_status()
            purchases = response.json()
            return render_template('checkout.html', purchases=purchases, admin=is_admin())

    except requests.exceptions.RequestException as e:
        flash('An error occurred while processing your request. Please try again later.', 'danger')
        return redirect(url_for('cart'))


@app.route('/purchases', methods=['GET'])
def purchases():
    try:
        if not is_admin():
            flash('Only admin users can update games.', 'danger')
            return redirect(url_for('home'))

        response = requests.get(f'{backend_url}/getAllPurchases', headers=getHeaders())
        response.raise_for_status()
        purchases = response.json()

        total_revenue = sum(item['num_copies'] * item['price'] for item in purchases)

        return render_template('purchases.html', purchases=purchases, total_revenue=total_revenue, admin=is_admin())
    except requests.exceptions.HTTPError as http_err:
        flash('Failed to fetch purchases: invalid credentials')
    except requests.exceptions.ConnectionError as conn_err:
        flash('Connection error: please try again later')
    except requests.exceptions.Timeout as timeout_err:
        flash('Request timed out: please try again later')
    except requests.exceptions.RequestException as req_err:
        flash('An unexpected error occurred: please try again later')
    return redirect(url_for('home'))

@app.route('/logout')
def logout():
    session.pop('user', None)
    return redirect(url_for('home'))


if __name__ == '__main__':
    app.run(debug=True)
